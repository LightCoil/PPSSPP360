#pragma once
#include <string>
#include <vector>
#include <cstdint>

// -----------------------------------------------------------------------------
//  ISOFile — класс для чтения ISO9660 и CSO/ISZ (CISO) образов.
//  - Open(path)             — открыть .iso/.cso/.isz (поддерживаются оба формата).
//  - GetFileSize(internal)  — получить размер файла внутри образа по пути "DIR/FILE.EXT".
//  - ReadFile(internal, buf, size) — считать именно тот файл в пользовательский буфер.
// -----------------------------------------------------------------------------
//  Поддерживаются имена и пути в кодировке ASCII (ISO9660 Level 1).
//  Реальные PSP-игры обычно используют именованные папки в верхнем регистре и расширения .PBP/.PNG.
// -----------------------------------------------------------------------------

class ISOFile {
public:
    ISOFile();
    ~ISOFile();

    //  Открыть ISO или CSO (ISZ) из файла. 
    //  Возвращает true, если удалось открыть и распознать заголовок.
    bool Open(const char *path);

    //  Размер внутреннего файла (например, "PSP_GAME/ICON0.PNG").
    //  Возвращает >0, если файл найден. Иначе — 0.
    int GetFileSize(const char *internalPath);

    //  Считать _весь_ внутренний файл (internalPath) в outBuffer (объём равен GetFileSize).
    //  Возвращает true, если всё успешно.
    bool ReadFile(const char *internalPath, void *outBuffer, int size);

    //  Закрыть и очистить все ресурсы.
    void Close();

private:
    //  Внутренние структуры
    bool        isCSO_;
    FILE       *fp_;
    uint64_t    fileSize_;      // общий размер ISO (не распакованный, если ISO) или CSO (compressed)
    uint32_t    blockSize_;     // всегда 2048 для ISO9660, для CSO обычно 2048
    uint32_t    indexCount_;    // число элементов csoIndex_ (=(fileSize_ / blockSize_) + 1)
    uint32_t   *csoIndex_;      // массив [indexCount_]индексов (CSO only)

    //  Если обычный ISO:
    //    csoIndex_ == nullptr, isCSO_ == false
    //  Если CSO/ISZ:
    //    isCSO_ == true, csoIndex_ указывает на выделенный массив, fp_ открыт на чтение.

    //  Внутренний парсер ISO9660:
    struct DirRecord {
        uint8_t  length;
        uint8_t  extAttrLen;
        uint32_t extentLBA;     // LBA начала (LE)
        uint32_t dataLen;       // Длина (LE)
        uint8_t  datetime[7];   // timestamp (не используется)
        uint8_t  flags;
        uint8_t  unitSize;
        uint8_t  gapSize;
        uint16_t volumeSeqNumber; // (LE)
        uint8_t  nameLen;
        char     name[256];     //  1 байт: длина, далее имя (без nul-терминатора)
    };

    //  Считать один блок (LBA) в целевой буфер (ptr, размер = blockSize_).
    //  Если ISO — просто fread с оффсетом (LBA * blockSize_).
    //  Если CSO — распаковать блок через zlib.
    bool ReadBlock(uint32_t LBA, uint8_t *ptr);

    //  Найти запись каталога по пути (разбить по '/'), вернуть DirRecord для файла.
    //  Если найден, заполняет outRecord и возвращает true.
    bool FindDirEntry(const std::string &path, DirRecord &outRecord);

    //  Рекурсивно обойти каталог: 
    //  - lbaRoot = LBA корневого дирректория (из PVD)
    //  - nameParts = компоненты пути: ["PSP_GAME","ICON0.PNG"]
    //  - idx = текущее смещение внутри каталога (в байтах)
    bool TraverseDirectory(uint32_t lbaRoot,
                           const std::vector<std::string> &nameParts,
                           size_t level,
                           DirRecord &outRecord);

    //  Считать первичный дескриптор тома (PVD), заполнить:
    //    - rootDirLBA, rootDirSize
    bool ReadPrimaryVolumeDescriptor(uint32_t &rootDirLBA, uint32_t &rootDirSize);

    //  Вспомогательный: прочитать little-endian 32-бит из массива байт.
    static uint32_t ReadLE32(const uint8_t *p) {
        return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
    }

    //  Вспомогательный: прочитать little-endian 16-бит из массива байт.
    static uint16_t ReadLE16(const uint8_t *p) {
        return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
    }

    //  Разбить путь "DIR1/DIR2/FILE.EXT" на вектор {"DIR1","DIR2","FILE.EXT"}.
    static void SplitPath(const std::string &path, std::vector<std::string> &outParts);

    //  Вычислить оффсет LBA->файла внутри CSO: 
    //    - находим в csoIndex_[LBA], csoIndex_[LBA+1], читаем区块 и распаковываем.
    bool DecompressCSOBlock(uint32_t blockIndex, uint8_t *outBuffer);
};
