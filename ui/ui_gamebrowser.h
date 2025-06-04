#pragma once
#include "ui_element.h"
#include <vector>
#include <string>

class UIGameBrowser : public UIElement {
public:
    UIGameBrowser();
    void Render() override;
    void Update() override;
    void OnInput(uint32_t btn) override;

private:
    struct GameEntry {
        std::string path;
        std::string name;
        uint32_t    texId;
    };
    std::vector<GameEntry> games_;
    int selected_;

    void ScanDirectory();
};
