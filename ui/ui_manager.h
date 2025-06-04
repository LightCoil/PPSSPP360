#pragma once
#include <vector>
#include "ui_element.h"

class UIManager {
public:
    void Push(UIElement *e);
    void Pop();
    void Update();
    void Render();
    void Input(uint32_t button);

private:
    std::vector<UIElement*> stack_;
};
