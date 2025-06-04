#include "iso_util.h"
#include "iso_reader.h"
#include <cstdio>

bool ISO_ExtractFile(const std::string &isoPath, const std::string &internal, std::vector<uint8_t> &out) {
    ISOFile iso;
    if (!iso.Open(isoPath.c_str()))
        return false;
    int len = iso.GetFileSize(internal.c_str());
    if (len <= 0)
        return false;
    out.resize(len);
    return iso.ReadFile(internal.c_str(), out.data(), len);
}
