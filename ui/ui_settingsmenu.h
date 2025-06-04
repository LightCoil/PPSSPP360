#pragma once
#include "ui_element.h"
#include <vector>
#include <string>
#include <functional>

class UISettingsMenu : public UIElement {
public:
    UISettingsMenu();
    void Render() override;
    void Update() override;
    void OnInput(uint32_t btn) override;

private:
    int selected_;
    std::vector<std::string> labels_;
    std::vector<std::function<void()>> actions_;

    void UpdateLabels();
};
