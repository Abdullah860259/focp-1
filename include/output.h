#pragma once
#include <iostream>
#include <string>

extern const std::string RESET, BOLD, FG_BLACK, FG_WHITE, FG_CYAN, FG_GREEN, FG_YELLOW, FG_RED,
                         BG_BLUE, BG_CYAN, BG_YELLOW, BG_GREEN, BG_GRAY, BG_BLACK, BG_RED;

void output();
void printInstructions();
void printExitMessage();
void invalidCommandMessage();
void printBar(std::string text, std::string colour);
void printHeading(std::string text);
void printText(std::string text);
void printBullet(std::string text);
void printStep(int n, std::string text);