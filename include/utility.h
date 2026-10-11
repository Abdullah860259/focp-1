// utility.h - small text and input helpers used by every module.
// Definitions are in src/utility.cpp.
#pragma once

#include <string>

std::string cleanInput(const std::string& input);   // "  Help " -> "help"
std::string upperCase(const std::string& text);     // "full relief" -> "FULL RELIEF"
std::string shorten(const std::string& text, int maxLength);  // cuts long text, adds "..."
std::string readPassword();                         // like typing a line, but shows * (Windows)
void waitForEnter();                                // "Press Enter to continue..."