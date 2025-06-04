#include "ui_slider.h"
#include "ui_renderer.h"
#include "../locale/locale.h"

UISlider::UISlider(const std::string &label, int minVal, int maxVal, int initial,
                   std::function<void(int)> callback)
    : label_(label), minVal_(minVal), maxVal_(maxVal), value_(initial),
      selected_(0), callback_(callback) {}

void UISlider::Render() {
    std::string text = label_ + ": " + std::to_string(value_);
    UIRender_DrawText(text, 100, 100, 0xFFFFFFFF);
    // Рисуем линию-ползунок
    float x = 100.0f;
    float y = 140.0f;
    float w = 200.0f;
    float h = 8.0f;
    UIRender_DrawRect(x, y, w, h, 0xFF444444);
    float pos = x + ((value_ - minVal_) / float(maxVal_ - minVal_)) * w;
    UIRender_DrawRect(pos - 4, y - 4, 8, h + 8, 0xFFFFFF00);
}

void UISlider::Update() {
    // ничего не делаем
}

void UISlider::OnInput(uint32_t btn) {
    if (btn & BTN_LEFT) {
        if (value_ > minVal_) {
            value_--;
            callback_(value_);
        }
    } else if (btn & BTN_RIGHT) {
        if (value_ < maxVal_) {
            value_++;
            callback_(value_);
        }
    } else if (btn & (BTN_A | BTN_B)) {
        // Закончить редактирование, вернуться к предыдущему экрану UI
    }
}
