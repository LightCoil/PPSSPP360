#pragma once
#include "ui_element.h"
#include <vector>
#include <string>
#include <functional>

class UIMainMenu : public UIElement {
public:
    UIMainMenu();
    void Render() override;
    void Update() override;
    void OnInput(uint32_t btn) override;

private:
    int selected_;
    struct MenuItem {
        std::string label;
        std::function<void()> action;
    };
    std::vector<MenuItem> items_;
};
