#pragma once
#include <string>
#include <functional>

class UIElement {
public:
    virtual void Render() = 0;
    virtual void Update() = 0;
    virtual void OnInput(uint32_t btn) = 0;
    virtual ~UIElement() {}
};
