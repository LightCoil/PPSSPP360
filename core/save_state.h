#pragma once
#include <cstdint>

void SaveStateToSlot(int slot);
void LoadStateFromSlot(int slot);
void SaveScreenshot(const char *path);
