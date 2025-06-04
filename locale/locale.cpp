#include "locale.h"
#include <fstream>
#include <sstream>

static std::unordered_map<std::string, std::string> translations;
static std::string emptyStr = "";

void LoadLocale(const std::string &lang) {
    translations.clear();
    std::string path = (lang == "ru") ? "uda:/locale_ru.txt" : "uda:/locale_en.txt";
    std::ifstream f(path);
    if (!f) return;
    std::string line;
    while (std::getline(f, line)) {
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        translations[key] = val;
    }
}

const std::string &tr(const std::string &key) {
    auto it = translations.find(key);
    if (it != translations.end()) return it->second;
    return emptyStr;
}
