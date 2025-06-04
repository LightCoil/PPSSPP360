#include "ui_statesmenu.h"
#include "ui_renderer.h"
#include "ui_manager.h"
#include "../core/save_state.h"
#include "../gfx/texture_utils.h"
#include "../locale/locale.h"
#include <sys/stat.h>
#include <cstdio>
#include <ctime>
#include <vector>
#include <string>

extern UIManager g_UI;

static uint32_t LoadPNGFile(const std::string &path) {
    FILE *f = fopen(path.c_str(), "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    rewind(f);
    std::vector<uint8_t> buf(len);
    fread(buf.data(), 1, len, f);
    fclose(f);
    return LoadPNG(buf);
}

UIStatesMenu::UIStatesMenu(bool isSaving)
    : isSaving_(isSaving), selected_(0) {
    LoadSlotInfo();
}

void UIStatesMenu::LoadSlotInfo() {
    slots_.clear();
    for (int i = 0; i < 5; ++i) {
        SlotInfo s;
        s.name = tr("slot") + " " + std::to_string(i);
        s.exists = false;
        s.texId = 0;

        char fname[64];
        sprintf(fname, "uda:/state%d.bin", i);
        struct stat st;
        if (stat(fname, &st) == 0) {
            s.exists = true;
            time_t t = st.st_mtime;
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&t));
            s.time = buf;
        } else {
            s.time = "—";
        }

        sprintf(fname, "uda:/screen%d.png", i);
        s.texId = LoadPNGFile(fname);

        slots_.push_back(s);
    }
}

void UIStatesMenu::Render() {
    UIRender_DrawText(
        isSaving_ ? tr("save_state") : tr("load_state"),
        100, 40, 0xFFFFFFFF
    );

    for (int i = 0; i < slots_.size(); ++i) {
        const auto &slot = slots_[i];
        uint32_t color = (i == selected_) ? 0xFFFFFF00 : 0xFFAAAAAA;
        std::string label = slot.name + " [" + slot.time + "]";
        UIRender_DrawText(label, 100, 120 + i * 40, color);
    }

    const auto &s = slots_[selected_];
    if (s.texId) {
        DrawTexture(s.texId, 420, 100, 160, 90);
    }
}

void UIStatesMenu::Update() {
    // при входе сюда, слот уже загружен
}

void UIStatesMenu::OnInput(uint32_t btn) {
    if (btn & BTN_DOWN) {
        selected_ = (selected_ + 1) % slots_.size();
    } else if (btn & BTN_UP) {
        selected_ = (selected_ + slots_.size() - 1) % slots_.size();
    } else if (btn & BTN_A) {
        if (isSaving_) {
            SaveStateToSlot(selected_);
        } else {
            LoadStateFromSlot(selected_);
        }
        LoadSlotInfo(); // обновить после действия
        g_UI.Pop();
    } else if (btn & BTN_B) {
        g_UI.Pop();
    }
}
