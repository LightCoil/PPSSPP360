#include "ui_mainmenu.h"
#include "ui_renderer.h"
#include "ui_manager.h"
#include "ui_gamebrowser.h"
#include "ui_settingsmenu.h"
#include "../core/psp_cpu.h"
#include "../save_state.h"
#include "../config.h"
#include "../locale/locale.h"
#include "../ui/ui_pausemenu.h"

extern UIManager g_UI;

UIMainMenu::UIMainMenu() : selected_(0) {
    items_.push_back({ tr("menu.play"), []() {
        g_UI.Push(new UIGameBrowser());
    }});
    items_.push_back({ tr("menu.settings"), []() {
        g_UI.Push(new UISettingsMenu());
    }});
    items_.push_back({ tr("menu.exit"), []() {
        exit(0);
    }});
}

void UIMainMenu::Render() {
    UIRender_DrawText("PPSSPP360", 120, 40, 0xFFFFFFFF);
    for (int i = 0; i < items_.size(); ++i) {
        uint32_t color = (i == selected_) ? 0xFFFFFF00 : 0xFFAAAAAA;
        UIRender_DrawText(items_[i].label, 120, 120 + i * 40, color);
    }
}

void UIMainMenu::Update() {}

void UIMainMenu::OnInput(uint32_t btn) {
    if (btn & BTN_DOWN) {
        selected_ = (selected_ + 1) % items_.size();
    } else if (btn & BTN_UP) {
        selected_ = (selected_ + items_.size() - 1) % items_.size();
    } else if (btn & BTN_A) {
        items_[selected_].action();
    }
}
