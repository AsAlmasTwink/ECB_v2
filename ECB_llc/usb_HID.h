#pragma once
#include "pch.h"

constexpr auto pid = "PID_";
constexpr auto vid = "VID_";
bool ExtractHexValue(const std::string& str, const std::string& prefix, USHORT* value);
bool RegistrateInput(HWND wnd);