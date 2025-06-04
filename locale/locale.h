#pragma once
#include <string>
#include <unordered_map>

void LoadLocale(const std::string &lang);
const std::string &tr(const std::string &key);
