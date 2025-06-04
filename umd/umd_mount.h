#pragma once
#include <string>

// Монтировать UMD (на самом деле ISO/CSO)
bool UMD_Mount(const std::string &path);
void UMD_Unmount();
