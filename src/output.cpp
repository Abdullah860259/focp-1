// =================================================================
//  output.cpp - everything that draws on the screen
//
//  - ANSI colour codes and box-drawing characters
//  - Simple lines: bars, headings, text, bullets, numbered steps
//  - Boxed tables (printRule / rowStart / cell) and cards
//  - The instructions, goodbye and invalid-command screens
// =================================================================

#include "./../include/output.h"
#include "./../include/engine.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

// windows.h is only needed on Windows (to switch on colours); the others are for Linux/Mac
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

// ======================= Display settings =======================
int WIDTH = 64;          // width of bars and headings; set to 80 in printInstructions()
const int MARGIN = 4;    // left/right indent for inner content
const int CMD_W  = 16;   // width of the command "badge" column (fits "show_inventory")

// ======================= ANSI escape codes ======================
// Printing one of these changes the colour/style of everything after it,
// until RESET is printed. FG_ = text colour, BG_ = background colour.
const string RESET     = "\033[0m";
const string BOLD      = "\033[1m";
const string CLEAR     = "\033[2J\033[H";   // clear screen, cursor to top

const string FG_BLACK  = "\033[30m";
const string FG_WHITE  = "\033[97m";
const string FG_CYAN   = "\033[96m";
const string FG_GREEN  = "\033[92m";
const string FG_YELLOW = "\033[93m";
const string FG_RED    = "\033[91m";

const string BG_BLUE   = "\033[44m";
const string BG_CYAN   = "\033[46m";
const string BG_YELLOW = "\033[43m";
const string BG_GREEN  = "\033[42m";
const string BG_GRAY   = "\033[100m";
const string BG_BLACK  = "\033[40m";
const string BG_RED    = "\033[41m";
const string BLINK = "\033[5m";           // blinking text (used for low-stock warnings)
const string FG_GRAY = "\033[90m";
const string FG_BLUE = "\033[94m";

// ======================= Box characters =========================
// Written as raw bytes (UTF-8) so they work whatever the editor setting is.
const string DOT_FULL = "\xE2\x97\x8F";    // filled circle
const string DOT_EMPTY = "\xE2\x97\x8B";    // empty circle
const string LINE_H = "\xE2\x94\x80";      // horizontal line
const string LINE_V = "\xE2\x94\x82";      // vertical line
// Corners (top-left, top-right, bottom-left, bottom-right) and T-joins for table borders
const string CORNER_TL = "\xE2\x94\x8C", CORNER_TR = "\xE2\x94\x90";
const string CORNER_BL = "\xE2\x94\x94", CORNER_BR = "\xE2\x94\x98";
const string T_DOWN = "\xE2\x94\xAC", T_UP = "\xE2\x94\xB4";
const string T_RIGHT = "\xE2\x94\x9C", T_LEFT = "\xE2\x94\xA4", CROSS = "\xE2\x94\xBC";

// ======================= System helpers =========================

// Lets the Windows console understand ANSI colour codes
void enableColors() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
	  SetConsoleMode(h, mode | 0x0004);       // turn on colour codes
    SetConsoleOutputCP(CP_UTF8);                // so the box characters show correctly
#endif
}

// ======================= Drawing helpers ========================

// Full-width coloured bar with text in the centre
void printBar(string text, string style){
    int len = (int)text.length();
    // Split the empty space evenly on both sides to centre the text
    int leftPad  = (WIDTH - len) / 2;
    int rightPad = WIDTH - leftPad - len;
    if (leftPad < 0)  leftPad = 0;
    if (rightPad < 0) rightPad = 0;

    cout << style << setw(leftPad) << "" << text << setw(rightPad) << ""
         << RESET << "\n";
}

// Section heading: full-width yellow bar, text on the left
void printHeading(string title){
    cout << "\n"
         << BG_YELLOW << FG_BLACK << BOLD
         << setw(MARGIN) << "" << left << setw(WIDTH - MARGIN) << ">> " + title
         << right << RESET << "\n\n";
}

// Normal paragraph line, indented
void printText(string text){
    cout << setw(MARGIN) << "" << text << "\n";
}

// Bullet point with a coloured marker
void printBullet(string text){
    cout << setw(MARGIN + 2) << "" << FG_CYAN << BOLD << "* " << RESET
         << text << "\n";
}

// Numbered step with a coloured number
void printStep(int n, string text){
    cout << setw(MARGIN + 2) << "" << FG_YELLOW << BOLD << n << "." << RESET
         << " " << text << "\n";
}

// ======================= Tables and cards =======================
static const string INDENT = "  ";         // left indent of tables and cards
static const int CARD_LABEL_W = 14;        // width of the label column inside cards
static const int PAIR_LABEL_W = 18;        // card pair: label + dots
static const int PAIR_SIDE_W = 34;        // card pair: width of one side

// text repeated several times, e.g. repeatText("-", 3) is "---"
static string repeatText(const string& text, int times) {
    string result = "";
    for (int i = 0; i < times; i++)
	  result += text;
    return result;
}

// Small grey line, used under headings
void printSubtitle(const string& text) {
    cout << INDENT << FG_GRAY << text << RESET << "\n";
}

// A horizontal table line. kind: 't' = top, 'm' = middle, 'b' = bottom
void printRule(char kind, int w1, int w2, int w3, int w4, int w5) {
    string left, mid, right;
    if (kind == 't') {                 // top line
	  left = CORNER_TL;  mid = T_DOWN;  right = CORNER_TR;
    }
    else if (kind == 'm') {          // middle line
	  left = T_RIGHT;    mid = CROSS;   right = T_LEFT;
    }
    else {                          // bottom line
	  left = CORNER_BL;  mid = T_UP;    right = CORNER_BR;
    }

    // Each column is its width + 2 (one space on each side of the text)
    string line = left + repeatText(LINE_H, w1 + 2);
    if (w2 > 0) line += mid + repeatText(LINE_H, w2 + 2);
    if (w3 > 0) line += mid + repeatText(LINE_H, w3 + 2);
    if (w4 > 0) line += mid + repeatText(LINE_H, w4 + 2);
    if (w5 > 0) line += mid + repeatText(LINE_H, w5 + 2);
    cout << INDENT << FG_BLUE << line + right << RESET << "\n";
}

// Start of a table row (the left border)
string rowStart() {
    return INDENT + FG_BLUE + LINE_V + RESET;
}

// One table cell, with its right border. Only the text is coloured.
// The text is cut if it is longer than the column, and padded with spaces if shorter.
string cell(const string& text, int width, Align align, const string& colour) {
    string shown = text.substr(0, width);
    int space = width - (int)shown.length();        // empty room left in the column

    int before = 0;                                 // spaces to put before the text
    if (align == ALIGN_RIGHT)
	  before = space;
    else if (align == ALIGN_CENTER)
	  before = space / 2;

    return " " + string(before, ' ') + colour + shown + RESET
	  + string(space - before, ' ') + " " + FG_BLUE + LINE_V + RESET;
}

// A table cell that holds a level bar: 'filled' squares in the given colour, the rest grey.
string barCell(int filled, int width, const string& colour) {
    return " " + colour + string(filled, ' ') + BG_GRAY + string(width - filled, ' ') + RESET
	  + " " + FG_BLUE + LINE_V + RESET;
}

// ---------- Cards ----------
static int    cardWidth = 76;       // set by cardTop, used by the other card functions
static string cardBorder = FG_BLUE;

// Top border of a card with its title:  +- TITLE ---------+
void cardTop(const string& title, int width, const string& border, const string& titleColour) {
    cardWidth = width;
    cardBorder = border;
    cout << INDENT << border << CORNER_TL << LINE_H << " " << RESET
	  << titleColour << title << RESET
	  << border << " " << repeatText(LINE_H, width - (int)title.length() - 5) << CORNER_TR << RESET << "\n";
}

// One line inside the card. 'visibleLength' = how many characters you can SEE
// (colour codes don't count); it is used to place the right border.
void cardLine(const string& content, int visibleLength) {
    int pad = cardWidth - 4 - visibleLength;
    if (pad < 0) pad = 0;
    cout << INDENT << cardBorder << LINE_V << RESET << " " << content
	  << string(pad, ' ') << " " << cardBorder << LINE_V << RESET << "\n";
}

// A line of plain coloured text inside a card (cut if it is too long)
void cardText(const string& text, const string& colour) {
    string shown = text.substr(0, cardWidth - 4);
    cardLine(colour + shown + RESET, (int)shown.length());
}

// An empty line inside a card
void cardBlank() {
    cardLine("", 0);
}

// One half of a pair line:  "Label ......... Value"
static string pairSide(const string& label, const string& value, const string& colour) {
    if (label == "")
	  return string(PAIR_SIDE_W, ' ');
    string dots = label + " ";
    // Fill with dots up to a fixed width so all the values line up
    while ((int)dots.length() < PAIR_LABEL_W)
	  dots += ".";
    int used = (int)dots.length() + 1 + (int)value.length();
    return FG_GRAY + dots + RESET + " " + colour + BOLD + value + RESET
	  + string(PAIR_SIDE_W - used, ' ');
}

// Two "Label .... Value" items side by side. Use "" as a label to leave a side empty.
void cardPair(const string& label1, const string& value1, const string& colour1,
    const string& label2, const string& value2, const string& colour2) {
    cardLine(pairSide(label1, value1, colour1) + "  " + pairSide(label2, value2, colour2),
	  2 * PAIR_SIDE_W + 2);
}

// "Label        text" - the label is white, the text has the colour you give
void cardLabelled(const string& label, const string& text, const string& colour) {
    cardLine(FG_WHITE + label + string(CARD_LABEL_W - label.length(), ' ') + RESET + colour + text + RESET,
	  CARD_LABEL_W + (int)text.length());
}

// "Need score   [coloured bar]  24 / 25" - one square per point
void cardMeter(const string& label, int score, int maxScore, const string& fill) {
    // One coloured square for every point scored, grey squares for the rest
    string bar = "";
    for (int point = 1; point <= maxScore; point++)
	  bar += (point <= score ? fill : BG_GRAY) + " ";
    string numbers = to_string(score) + " / " + to_string(maxScore);
    cardLine(FG_WHITE + label + string(CARD_LABEL_W - label.length(), ' ') + RESET
	  + bar + RESET + "  " + BOLD + numbers + RESET,
	  CARD_LABEL_W + maxScore + 2 + (int)numbers.length());
}

// " OK  text" or " NO  text"; long text continues on the next line
void cardCheck(bool ok, const string& text) {
    const int BADGE_W = 6;                       // " OK " + two spaces
    string badge = ok ? BG_GREEN + FG_BLACK + BOLD + " OK " + RESET
	  : BG_RED + FG_WHITE + BOLD + " NO " + RESET;
    int room = cardWidth - 4 - BADGE_W;

    // Print the text in pieces that fit the card, breaking at a space where possible
    string rest = text;
    bool firstLine = true;
    while (rest != "") {
	  string part = rest;
	  if ((int)rest.length() > room) {
		size_t cut = rest.rfind(' ', room);   // last space that still fits
		if (cut == string::npos || cut == 0) {
		    part = rest.substr(0, room);
		    rest = rest.substr(room);
		}
		else {
		    part = rest.substr(0, cut);
		    rest = rest.substr(cut + 1);
		}
	  }
	  else {
		rest = "";
	  }
	  // Badge on the first line only; later lines are indented to line up under the text
	  string front = firstLine ? badge + "  " : string(BADGE_W, ' ');
	  cardLine(front + part, BADGE_W + (int)part.length());
	  firstLine = false;
    }
}

// Bottom border of the current card
void cardBottom() {
    cout << INDENT << cardBorder << CORNER_BL << repeatText(LINE_H, cardWidth - 2)
	  << CORNER_BR << RESET << "\n";
}

// One row of the command menu table
void printMenuRow(const string& name, const string& description, const string& descriptionColour = FG_WHITE) {
    cout << rowStart() << cell(name, 16, ALIGN_LEFT, FG_GREEN + BOLD)
	  << cell(description, 54, ALIGN_LEFT, descriptionColour) << "\n";
}

// ======================= Screens ================================

// Welcome screen: banner, introduction, command menu, how to use, and an example.
// Shown when the program starts and by the "instructions" command.
void printInstructions(){

    enableColors();
    WIDTH = 80;   // fixed width, so the tables and bars always line up
    cout << CLEAR;                    // wipe the screen before drawing

    // ---------- Banner ----------
    printBar("", BG_BLUE);
    printBar("RELIEF SUPPLY DISTRIBUTION SYSTEM", BG_BLUE + FG_WHITE + BOLD);
    printBar("", BG_BLUE);
    printBar("Getting help where it's needed, fairly.", BG_CYAN + FG_BLACK);

    // ---------- Introduction ----------
    printHeading("INTRODUCTION");
    printText("After a disaster such as a flood, trucks and relief");
    printText("supplies are limited, and many areas ask for help at");
    printText("once. This program decides how much relief each area");
    printText("should get, without running out of stock too quickly.");
    cout << "\n";
    printText(BOLD + string("For every request the system:") + RESET);
    printBullet("Asks a few questions about the affected area");
    printBullet("Calculates a need score from the answers");
    printBullet("Picks a relief package that matches the need");
    printBullet("Sends a smaller package if stock is too low");
    printBullet("Updates the stock and saves the request");

    // ---------- Commands ----------
    printHeading("COMMAND MENU");
    printRule('t', 16, 54);
    cout << rowStart() << cell("COMMAND", 16, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("DESCRIPTION", 54, ALIGN_LEFT, FG_CYAN + BOLD) << "\n";
    printRule('m', 16, 54);
    printMenuRow("help", "Request relief supplies for an area");
    printMenuRow("resources", "Print all supplies and quantities");
    printMenuRow("update", "Add or remove stock  [admin only]", FG_YELLOW);
    printMenuRow("status", "Show summary of distributions");
    printMenuRow("instructions", "Show this instructions screen");
    printMenuRow("exit", "Exit the program");
    printRule('b', 16, 54);

    // ---------- How to use ----------
    printHeading("HOW TO USE");
    printStep(1, "Type a command and press Enter.");
    printStep(2, "Answer the questions about the affected area (area, damage level, etc)");
    printStep(3, "Quantities must be either zero or positive integers.");
    printStep(4, "You cannot request more than what is in stock.");

    // ---------- Example ----------
    // A real request and its result (score 21 / 25 -> Full Relief)
    printHeading("EXAMPLE");
    cardTop("AN EXAMPLE");
    cardText("Enter a command: help", FG_GREEN + BOLD);
    cardText("  Area name : Muzaffarabad", FG_WHITE);
    cardText("  Damage level (low/medium/high) : high", FG_WHITE);
    cardText("  Families affected (10-200) : 150", FG_WHITE);
    cardText("  Access route (open/damaged): open", FG_WHITE);
    cardText("  Distance to the area in km (1-500): 80", FG_WHITE);
    cardText("  Water scarcity (low/medium/high): high", FG_WHITE);
    cardText("  Food availability (low/medium/high): low", FG_WHITE);
    cardText("  Number of injured people (0-1000): 120", FG_WHITE);
    cardText("  Previous aid received (yes/no): no", FG_WHITE);
    cardText("  DISPATCHED: A. Full Relief", FG_YELLOW + BOLD);
    cardBottom();                        

    // ---------- Footer ----------
    cout << "\n";
    printBar("Type 'instructions' at any time to see this screen again.",
             BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";
}


// Goodbye card shown after the user confirms "exit"
void printExitMessage(){
    cout << "\n";
    cardTop("GOODBYE!", 66);
    cardText("Thank you for using this Relief Supply Distribution System.", FG_WHITE);
    cardText("Together we can make a difference.", FG_GRAY);
    cout << "\n";
}

// Yellow warning bar for a command the program does not know
void invalidCommandMessage(){
    cout << "\n";
    printBar("Invalid command. Type 'instructions' to see the list of commands.",
             BG_YELLOW + FG_BLACK + BOLD);
    cout << "\n";
}
