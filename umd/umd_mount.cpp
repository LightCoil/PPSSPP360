#include "umd_mount.h"
#include "../core/iso_reader.h"
#include <iostream>

static ISOFile *g_iso = nullptr;

bool UMD_Mount(const std::string &path) {
    if (g_iso) return false;
    g_iso = new ISOFile();
    if (!g_iso->Open(path.c_str())) {
        delete g_iso;
        g_iso = nullptr;
        return false;
    }
    return true;
}

void UMD_Unmount() {
    if (g_iso) {
        g_iso->Close();
        delete g_iso;
        g_iso = nullptr;
    }
}
