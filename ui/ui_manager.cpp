#include "ui_manager.h"

void UIManager::Push(UIElement *e) {
    stack_.push_back(e);
}

void UIManager::Pop() {
    if (!stack_.empty()) {
        delete stack_.back();
        stack_.pop_back();
    }
}

void UIManager::Update() {
    if (!stack_.empty()) {
        stack_.back()->Update();
    }
}

void UIManager::Render() {
    if (!stack_.empty()) {
        stack_.back()->Render();
    }
}

void UIManager::Input(uint32_t button) {
    if (!stack_.empty()) {
        stack_.back()->OnInput(button);
    }
}
