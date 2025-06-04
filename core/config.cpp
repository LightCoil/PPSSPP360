#include "config.h"
#include <cstdio>

Config g_Config;

void Config::Load() {
    FILE *f = fopen("uda:/ppsspp.cfg", "rb");
    if (!f) return;
    fread(this, sizeof(Config), 1, f);
    fclose(f);
}

void Config::Save() {
    FILE *f = fopen("uda:/ppsspp.cfg", "wb");
    if (!f) return;
    fwrite(this, sizeof(Config), 1, f);
    fclose(f);
}
