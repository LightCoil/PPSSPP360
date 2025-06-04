#pragma once

enum class Resolution { R480p, R720p };
enum class Language { Russian, English };

struct Config {
    Resolution resolution = Resolution::R720p;
    bool audioEnabled = true;
    Language language = Language::Russian;

    void Load();
    void Save();
};

extern Config g_Config;
