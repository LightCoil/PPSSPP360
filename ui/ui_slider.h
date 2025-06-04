#pragma once
#include "ui_element.h"
#include <functional>
#include <string>

// Простейший слайдер (например, для громкости)
class UISlider : public UIElement {
public:
    UISlider(const std::string &label, int minVal, int maxVal, int initial,
             std::function<void(int)> callback);

    void Render() override;
    void Update() override;
    void OnInput(uint32_t btn) override;

private:
    std::string label_;
    int minVal_, maxVal_, value_;
    int selected_;
    std::function<void(int)> callback_;
};
