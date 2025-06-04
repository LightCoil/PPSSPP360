#include "iso_reader.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <zlib.h>

// ----------------------------------------------------------------------------
//  Реализация ISOFile
// ----------------------------------------------------------------------------

ISOFile::ISOFile()
    : isCSO_(false)
    , fp_(nullptr)
    , fileSize_(0)
    , blockSize_(2048)
    , indexCount_(0)
    , csoIndex_(nullptr)
{}

ISOFile::~ISOFile() {
    Close();
}

void ISOFile::Close() {
    if (fp_) {
        fclose(fp_);
        fp_ = nullptr;
    }
    if (csoIndex_) {
        free(csoIndex_);
        csoIndex_ = nullptr;
    }
    fileSize_   = 0;
    indexCount_ = 0;
    isCSO_      = false;
}

//  Открыть .iso или .cso/.isz (CISO).
bool ISOFile::Open(const char *path) {
    Close();

    fp_ = fopen(path, "rb");
    if (!fp_) return false;

    // Узнать общий размер файла на диске:
    fseek(fp_, 0, SEEK_END);
    uint64_t realSize = ftell(fp_);
    fseek(fp_, 0, SEEK_SET);

    // Считать первые 4 байта, чтобы проверить CSO-магик:
    char magic[4];
    if (fread(magic,1,4,fp_) != 4) {
        Close();
        return false;
    }
    rewind(fp_);

    if (memcmp(magic, "CISO", 4) == 0) {
        // Это CSO или ISZ (CISO). Прочитаем заголовок:
        // struct CISOHeader {
        //   char   magic[4];      // "CISO"
        //   uint64_t totalBytes;  // полный раскомпресс. размер (для ISZ может совпадать с размером файла)
        //   uint32_t blockSize;   // обычно 2048
        //   uint8_t  ver;         // версия (часто 1)
        //   uint8_t  align;       // степени выравнивания
        //   uint8_t  rsvd[2];     // зарезервировано
        // };
        struct {
            char     magic[4];
            uint64_t totalBytes;
            uint32_t blockSize;
            uint8_t  ver;
            uint8_t  align;
            uint8_t  rsvd[2];
        } hdr;
        if (fread(&hdr, sizeof(hdr), 1, fp_) != 1) {
            Close();
            return false;
        }
        blockSize_ = hdr.blockSize;
        fileSize_  = hdr.totalBytes;  // uncompressed size
        
        // Количество блоков (минимально): (uncompressedBytes + blockSize - 1) / blockSize
        indexCount_ = (uint32_t)((fileSize_ + blockSize_ - 1) / blockSize_) + 1;
        // Выделим массив индексов:
        csoIndex_ = (uint32_t*)malloc(indexCount_ * sizeof(uint32_t));
        if (!csoIndex_) {
            Close();
            return false;
        }
        // Считать таблицу индексов (indexCount элементов, uint32)
        if (fread(csoIndex_, sizeof(uint32_t), indexCount_, fp_) != indexCount_) {
            Close();
            return false;
        }
        // Переводим все индексы из big-endian в host (если нужно):
        // На CSO они уже little-endian (обычно), но проверить:
        // Обычно csoIndex_[i] = (offset <<  3) | (compressed?1:0).
        // Предполагаем, что хост little-endian, поэтому нет необходимости бswap.
        isCSO_ = true;
        return true;
    }
    else {
        // Это обычный .iso. Просто узнаем полный размер:
        blockSize_   = 2048;
        fileSize_    = realSize;
        indexCount_  = 0;
        isCSO_       = false;
        return true;
    }
}

//  Считать один блок (LBA) в ptr (ptr размером blockSize_).
bool ISOFile::ReadBlock(uint32_t LBA, uint8_t *ptr) {
    if (!isCSO_) {
        // Простой ISO — оффсет = LBA * 2048
        uint64_t offset = (uint64_t)LBA * blockSize_;
        fseek(fp_, offset, SEEK_SET);
        size_t readBytes = fread(ptr, 1, blockSize_, fp_);
        return (readBytes == blockSize_);
    } else {
        // CSO: нужно распаковать один блок:
        return DecompressCSOBlock(LBA, ptr);
    }
}

//  Распаковка одного CSO-блока (или копирование, если не сжат).
bool ISOFile::DecompressCSOBlock(uint32_t blockIndex, uint8_t *outBuffer) {
    if (!isCSO_ || !csoIndex_) return false;
    if (blockIndex + 1 >= indexCount_) return false;

    uint32_t idxA = csoIndex_[blockIndex];
    uint32_t idxB = csoIndex_[blockIndex + 1];
    uint64_t fileOfsA = (uint64_t)(idxA & 0x7FFFFFFF) << (csoIndex_[blockIndex] & 0x80000000 ? 0 : blockSize_ == 2048 ?  11 : 0);
    uint64_t fileOfsB = (uint64_t)(idxB & 0x7FFFFFFF) << (csoIndex_[blockIndex+1] & 0x80000000 ? 0 : blockSize_ == 2048 ?  11 : 0);
    uint32_t  blkSize = (uint32_t)( (idxB & 0x7FFFFFFF) - (idxA & 0x7FFFFFFF) );

    // Флаг сжатия — старший бит индекса:
    bool isPacked = ((idxA & 0x80000000) == 0);

    if (!isPacked) {
        // Блок не сжат: просто читаем raw-данные размером blockSize_
        fseek(fp_, fileOfsA, SEEK_SET);
        size_t r = fread(outBuffer, 1, blockSize_, fp_);
        return (r == blockSize_);
    } else {
        // Зашифрованный/сжатый блок: DEFLATE 
        // blkSize = (idxB & 0x7FFFFFFF) - (idxA & 0x7FFFFFFF)
        std::vector<uint8_t> compBuf(blkSize);
        fseek(fp_, fileOfsA, SEEK_SET);
        size_t r = fread(compBuf.data(), 1, blkSize, fp_);
        if (r != blkSize) return false;

        // Декодируем через zlib (inflate)
        z_stream strm;
        memset(&strm, 0, sizeof(strm));
        strm.next_in   = compBuf.data();
        strm.avail_in  = blkSize;
        strm.next_out  = outBuffer;
        strm.avail_out = blockSize_;

        if (inflateInit2(&strm, -15) != Z_OK) { // raw inflate
            return false;
        }
        int ret = inflate(&strm, Z_FINISH);
        inflateEnd(&strm);
        return (ret == Z_STREAM_END);
    }
}

//  Прочитать PVD (Primary Volume Descriptor), получить LBA/Размер корня.
bool ISOFile::ReadPrimaryVolumeDescriptor(uint32_t &rootDirLBA, uint32_t &rootDirSize) {
    // PVD находится по сдвигу 16*2048 = 32768 байт.
    const uint32_t PVD_LBA = 16;
    uint8_t buffer[2048];
    if (!ReadBlock(PVD_LBA, buffer)) return false;

    // Проверяем дескриптор: buffer[0] == 0x01, "CD001"
    if (buffer[0] != 0x01 || 
        memcmp(buffer + 1, "CD001", 5) != 0 || 
        buffer[6] != 0x01) {
        return false;
    }
    // Root Directory Record начинается с байта 156
    uint8_t *dirRec = buffer + 156;
    rootDirLBA  = ReadLE32(dirRec + 2);
    rootDirSize = ReadLE32(dirRec + 10);
    return true;
}

//  Разбить путь по '/' (без удаления лишних '/').
//  Пример: "PSP_GAME/ICON0.PNG" → {"PSP_GAME","ICON0.PNG"}.
void ISOFile::SplitPath(const std::string &path, std::vector<std::string> &outParts) {
    outParts.clear();
    size_t start = 0;
    while (start < path.size()) {
        size_t pos = path.find('/', start);
        if (pos == std::string::npos) {
            outParts.push_back(path.substr(start));
            break;
        } else {
            outParts.push_back(path.substr(start, pos - start));
            start = pos + 1;
        }
    }
}

//  Найти запись каталога (DirRecord) для заданного пути.
//  Если найден — записать в outRecord и вернуть true.
bool ISOFile::FindDirEntry(const std::string &internalPath, DirRecord &outRecord) {
    uint32_t rootLBA, rootSize;
    if (!ReadPrimaryVolumeDescriptor(rootLBA, rootSize)) return false;

    std::vector<std::string> parts;
    SplitPath(internalPath, parts);
    // Начинаем поиск с корня
    return TraverseDirectory(rootLBA, parts, 0, outRecord);
}

//  Рекурсивный обход каталога.
//   - lbaRoot: LBA начала текущего каталога
//   - nameParts: {"PSP_GAME","ICON0.PNG"}
//   - level: текущая глубина (индекс в nameParts), 
//            если level == nameParts.size()-1, ищем именно файл, иначе — папку.
//   - outRecord: результат этого уровня.
//  Возвращает true, если найден завершённый путь (файл) и заполнен outRecord.
bool ISOFile::TraverseDirectory(uint32_t lbaRoot,
                                const std::vector<std::string> &nameParts,
                                size_t level,
                                DirRecord &outRecord) {
    // Корневой каталог может занимать несколько блоков (rootSize), но обычно довольно мал.
    // Читаем блок за блоком, пока не найдём нужный entry:
    uint32_t        offsetWithinBlock = 0;
    uint32_t        blockNum = lbaRoot;
    std::vector<uint8_t> blockBuf(blockSize_);

    while (true) {
        if (offsetWithinBlock == 0) {
            // Читать новый блок
            if (!ReadBlock(blockNum, blockBuf.data())) return false;
        }
        // В текущем блоке читаем запись
        DirRecord rec;
        rec.length        = blockBuf[offsetWithinBlock + 0];
        rec.extAttrLen    = blockBuf[offsetWithinBlock + 1];
        rec.extentLBA     = ReadLE32(blockBuf.data() + offsetWithinBlock + 2);
        rec.dataLen       = ReadLE32(blockBuf.data() + offsetWithinBlock + 10);
        // skip datetime (7 байт)
        rec.flags         = blockBuf[offsetWithinBlock + 25];
        rec.unitSize      = blockBuf[offsetWithinBlock + 26];
        rec.gapSize       = blockBuf[offsetWithinBlock + 27];
        rec.volumeSeqNumber = ReadLE16(blockBuf.data() + offsetWithinBlock + 28);
        rec.nameLen       = blockBuf[offsetWithinBlock + 32];
        memset(rec.name, 0, sizeof(rec.name));
        memcpy(rec.name, blockBuf.data() + offsetWithinBlock + 33, rec.nameLen);

        if (rec.length == 0) {
            // Конец блока: переходим к следующему блоку текущего каталога
            blockNum++;
            offsetWithinBlock = 0;
            continue;
        }

        // Имя: может содержать ';1' в конце версии. Отрежем ";1"
        std::string nameStr(rec.name, rec.nameLen);
        size_t sep = nameStr.find(';');
        if (sep != std::string::npos) {
            nameStr = nameStr.substr(0, sep);
        }
        // ISO9660 имена в верхнем регистре; сравниваем как есть
        if (strcasecmp(nameStr.c_str(), nameParts[level].c_str()) == 0) {
            // Нашли нужный entry
            if (level + 1 == nameParts.size()) {
                // Это последний компонент — значит файл
                outRecord = rec;
                return true;
            } else {
                // Это промежуточный каталог: должен быть директорией (flags & 0x02)
                if (rec.flags & 0x02) {
                    // Переходим внутрь: читаем каталог rec.extentLBA (rec.dataLen байт)
                    return TraverseDirectory(rec.extentLBA, nameParts, level + 1, outRecord);
                } else {
                    return false;
                }
            }
        }

        // Переходим к следующей записи в текущем блоке
        offsetWithinBlock += rec.length;
    }
    // Никогда сюда не дойдёт
    return false;
}

//  Получить размер файла внутри ISO: 
//  - если нет, вернуть 0.
//  - иначе вернуть rec.dataLen.
int ISOFile::GetFileSize(const char *internalPath) {
    DirRecord rec;
    if (FindDirEntry(internalPath, rec)) {
        return (int)rec.dataLen;
    } else {
        return 0;
    }
}

//  Считать весь файл (internalPath) длиной size байт в outBuffer.
//  Алгоритм:
//   1) Найти DirRecord, получить rec.extentLBA и rec.dataLen.
//   2) Рассчитать, сколько блоков занимает файл: blkCount = (dataLen+blockSize_-1)/blockSize_.
//   3) Читать блоки по очереди (ReadBlock), копируя в outBuffer.
//   4) В последнем блоке обрезать лишние байты (если размер не кратен blockSize_).
bool ISOFile::ReadFile(const char *internalPath, void *outBuffer, int size) {
    DirRecord rec;
    if (!FindDirEntry(internalPath, rec)) return false;
    uint32_t lbaStart = rec.extentLBA;
    uint32_t dataLen  = rec.dataLen;
    if (dataLen != (uint32_t)size) {
        // передали неверный size
        return false;
    }
    uint32_t blkCount = (dataLen + blockSize_ - 1) / blockSize_;
    uint8_t *dst = (uint8_t*)outBuffer;
    std::vector<uint8_t> tmpBlk(blockSize_);

    for (uint32_t i = 0; i < blkCount; ++i) {
        if (!ReadBlock(lbaStart + i, tmpBlk.data())) return false;
        if (i + 1 < blkCount) {
            // полный блок
            memcpy(dst + (i * blockSize_), tmpBlk.data(), blockSize_);
        } else {
            // последний блок — обрезаем до dataLen % blockSize_
            uint32_t remain = dataLen - (i * blockSize_);
            memcpy(dst + (i * blockSize_), tmpBlk.data(), remain);
        }
    }
    return true;
}
