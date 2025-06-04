#include "ui_settingsmenu.h"
#include "ui_renderer.h"
#include "ui_manager.h"
#include "../core/config.h"
#include "../locale/locale.h"

extern UIManager g_UI;

UISettingsMenu::UISettingsMenu() : selected_(0) {
    actions_.push_back([]() {
        g_Config.resolution = (g_Config.resolution == Resolution::R720p) ? Resolution::R480p : Resolution::R720p;
    });
    actions_.push_back([]() {
        g_Config.audioEnabled = !g_Config.audioEnabled;
    });
    actions_.push_back([]() {
        g_Config.language = (g_Config.language == Language::Russian) ? Language::English : Language::Russian;
        LoadLocale(g_Config.language == Language::Russian ? "ru" : "en");
    });
    actions_.push_back([]() {
        g_Config.Save();
    });
    actions_.push_back([]() {
        g_UI.Pop();
    });
    UpdateLabels();
}

void UISettingsMenu::UpdateLabels() {
    labels_.clear();
    labels_.push_back(
        tr("menu.resolution") + ": " +
        (g_Config.resolution == Resolution::R720p ? "720p" : "480p")
    );
    labels_.push_back(
        tr("menu.audio") + ": " +
        (g_Config.audioEnabled ? "On" : "Off")
    );
    labels_.push_back(
        tr("menu.language") + ": " +
        (g_Config.language == Language::Russian ? "Русский" : "English")
    );
    labels_.push_back(tr("menu.savecfg"));
    labels_.push_back(tr("menu.back"));
}

void UISettingsMenu::Render() {
    UIRender_DrawText(tr("menu.settings"), 100, 40, 0xFFFFFFFF);
    for (int i = 0; i < labels_.size(); ++i) {
        uint32_t color = (i == selected_) ? 0xFFFFFF00 : 0xFFAAAAAA;
        UIRender_DrawText(labels_[i], 100, 120 + i * 30, color);
    }
}

void UISettingsMenu::Update() {}

void UISettingsMenu::OnInput(uint32_t btn) {
    if (btn & BTN_DOWN) {
        selected_ = (selected_ + 1) % labels_.size();
    } else if (btn & BTN_UP) {
        selected_ = (selected_ + labels_.size() - 1) % labels_.size();
    } else if (btn & BTN_A) {
        actions_[selected_]();
        UpdateLabels();
    } else if (btn & BTN_B) {
        g_UI.Pop();
    }
}
