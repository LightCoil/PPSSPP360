#include "ui_gamebrowser.h"
#include "ui_renderer.h"
#include "ui_manager.h"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include "../core/loader.h"
#include "../gfx/texture_utils.h"
#include "../core/iso_util.h"
#include "../locale/locale.h"

extern UIManager g_UI;

static uint32_t LoadIconFromISO(const std::string &isoPath) {
    std::vector<uint8_t> iconData;
    if (!ISO_ExtractFile(isoPath, "PSP_GAME/ICON0.PNG", iconData)) {
        return 0;
    }
    return LoadPNG(iconData);
}

UIGameBrowser::UIGameBrowser() : selected_(0) {
    ScanDirectory();
}

void UIGameBrowser::ScanDirectory() {
    games_.clear();
    const char *dirs[] = { "usb:/games", "uda:/games" };
    for (int d = 0; d < 2; ++d) {
        DIR *dir = opendir(dirs[d]);
        if (!dir) continue;
        struct dirent *ent;
        while ((ent = readdir(dir)) != nullptr) {
            std::string name = ent->d_name;
            if (name.size() > 4) {
                std::string ext = name.substr(name.size() - 4);
                for (auto &e : { ".iso", ".cso", ".ISO", ".CSO" }) {
                    if (ext == e) {
                        GameEntry ge;
                        ge.path = std::string(dirs[d]) + "/" + name;
                        ge.name = name;
                        ge.texId = LoadIconFromISO(ge.path);
                        games_.push_back(ge);
                        break;
                    }
                }
            }
        }
        closedir(dir);
        if (!games_.empty()) break;
    }
}

void UIGameBrowser::Render() {
    UIRender_DrawText(tr("menu.play"), 100, 40, 0xFFFFFFFF);
    if (games_.empty()) {
        UIRender_DrawText(tr("no_games"), 100, 120, 0xFF888888);
        return;
    }

    // Список файлов
    int y = 120;
    for (int i = 0; i < games_.size(); ++i) {
        uint32_t color = (i == selected_) ? 0xFFFFFF00 : 0xFFAAAAAA;
        UIRender_DrawText(games_[i].name, 100, y + i * 30, color);
    }

    // Превью иконка
    const auto &g = games_[selected_];
    if (g.texId) {
        DrawTexture(g.texId, 400, 100, 160, 96);
    }
}

void UIGameBrowser::Update() {}

void UIGameBrowser::OnInput(uint32_t btn) {
    if (games_.empty()) {
        if (btn & BTN_B) g_UI.Pop();
        return;
    }
    if (btn & BTN_DOWN) {
        selected_ = (selected_ + 1) % games_.size();
    } else if (btn & BTN_UP) {
        selected_ = (selected_ + games_.size() - 1) % games_.size();
    } else if (btn & BTN_A) {
        LaunchISO(games_[selected_].path.c_str());
    } else if (btn & BTN_B) {
        g_UI.Pop();
    }
}
