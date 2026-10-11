// output.h - colours and every function that draws on the screen.
// Definitions are in src/output.cpp.
#pragma once
#include <iostream>
#include <string>

// ---------- Colours ----------
extern const std::string RESET, BOLD, BLINK,
FG_BLACK, FG_WHITE, FG_GRAY, FG_BLUE, FG_CYAN, FG_GREEN, FG_YELLOW, FG_RED,
BG_BLUE, BG_CYAN, BG_YELLOW, BG_GREEN, BG_GRAY, BG_BLACK, BG_RED;

// ---------- Box characters and circles ----------
extern const std::string DOT_FULL;    // filled circle
extern const std::string DOT_EMPTY;   // empty circle
extern const std::string LINE_H;      // a short horizontal line

// How text is placed inside a table cell
enum Align { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

// ---------- Screens ----------
void printInstructions();
void printExitMessage();
void invalidCommandMessage();

// ---------- Simple lines ----------
void printBar(std::string text, std::string colour);
void printHeading(std::string text);
void printText(std::string text);
void printBullet(std::string text);
void printStep(int n, std::string text);
void printSubtitle(const std::string& text);        // small grey line

// ---------- Tables ----------
// printRule draws a horizontal table line: 't' = top, 'm' = middle, 'b' = bottom.
// Give it the width of each column (a width of 0 means "no more columns").
void printRule(char kind, int w1, int w2 = 0, int w3 = 0, int w4 = 0, int w5 = 0);
std::string rowStart();
std::string cell(const std::string& text, int width, Align align, const std::string& colour);
std::string barCell(int filled, int width, const std::string& colour);   // a cell holding a level bar

// ---------- Cards (boxes with a title) ----------
void cardTop(const std::string& title, int width = 76,
    const std::string& border = FG_BLUE, const std::string& titleColour = FG_CYAN + BOLD);
void cardLine(const std::string& content, int visibleLength);
void cardText(const std::string& text, const std::string& colour);
void cardBlank();
void cardPair(const std::string& label1, const std::string& value1, const std::string& colour1,
    const std::string& label2, const std::string& value2, const std::string& colour2);
void cardLabelled(const std::string& label, const std::string& text, const std::string& colour);
void cardMeter(const std::string& label, int score, int maxScore, const std::string& fill);
void cardCheck(bool ok, const std::string& text);
void cardBottom();