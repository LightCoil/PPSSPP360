#pragma once
#include "ui_element.h"
#include <vector>
#include <string>

class UIStatesMenu : public UIElement {
public:
    UIStatesMenu(bool isSaving);
    void Render() override;
    void Update() override;
    void OnInput(uint32_t btn) override;

private:
    struct SlotInfo {
        std::string name;
        std::string time;
        uint32_t    texId;
        bool        exists;
    };
    bool isSaving_;
    int selected_;
    std::vector<SlotInfo> slots_;
    void LoadSlotInfo();
};
