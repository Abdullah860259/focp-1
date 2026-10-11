#pragma once

#include <string>

std::string cleanInput(const std::string& input);
std::string upperCase(const std::string& text);     // "full relief" -> "FULL RELIEF"
std::string shorten(const std::string& text, int maxLength);  // cuts long text, adds "..."
std::string readPassword();                         // like typing a line, but shows * (Windows)
void waitForEnter();                                // "Press Enter to continue..."