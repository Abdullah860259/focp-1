#include "./../include/output.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

// ======================= Display settings =======================
int WIDTH = 64;          // updated to the terminal width at runtime
const int MARGIN = 4;    // left/right indent for inner content
const int CMD_W  = 13;   // width of the command "badge" column

// ======================= ANSI escape codes ======================
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

// ======================= System helpers =========================

// Lets the Windows console understand ANSI colour codes
void enableColors(){
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004);   // ENABLE_VIRTUAL_TERMINAL_PROCESSING
#endif
}

// Returns the current width of the terminal in characters
int getTerminalWidth(){
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return info.srWindow.Right - info.srWindow.Left + 1;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
#endif
    return 64;
}

// ======================= Drawing helpers ========================

// Full-width coloured bar with text in the centre
void printBar(string text, string style){
    int len = (int)text.length();
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

// Width left for the description column so the table fits exactly
int descWidth(){
    return WIDTH - 2 * MARGIN - CMD_W - 4;
}

// Header row of the commands table
void printTableHeader(){
    cout << setw(MARGIN) << "" << BG_BLUE << FG_WHITE << BOLD
         << " " << left << setw(CMD_W) << "COMMAND" << " "
         << "  " << setw(descWidth()) << "DESCRIPTION"
         << right << RESET << "\n";
}

// One row of the commands table; 'shaded' gives zebra striping
void printCommand(string cmd, string desc, bool shaded){
    string rowBg = shaded ? BG_GRAY : BG_BLACK;

    cout << setw(MARGIN) << ""
         << BG_GREEN << FG_BLACK << BOLD
         << " " << left << setw(CMD_W) << cmd << " " << RESET
         << rowBg << FG_WHITE
         << "  " << setw(descWidth()) << desc
         << right << RESET << "\n";
}

// One line inside the mini "terminal window" used for the example
void printTerminalLine(string text, string colour){
    int inner = WIDTH - 2 * MARGIN;
    cout << setw(MARGIN) << "" << BG_BLACK << colour
         << "  " << left << setw(inner - 2) << text
         << right << RESET << "\n";
}

// ======================= Main function ==========================

void printInstructions(){

    enableColors();
    WIDTH = getTerminalWidth() - 1;   // -1 so lines don't wrap
    if (WIDTH < 60)  WIDTH = 60;
    if (WIDTH > 100) WIDTH = 100;

    cout << CLEAR;

    // ---------- Banner ----------
    printBar("", BG_BLUE);
    printBar("RELIEF SUPPLY DISTRIBUTION SYSTEM", BG_BLUE + FG_WHITE + BOLD);
    printBar("", BG_BLUE);
    printBar("Getting help where it's needed, fairly.", BG_CYAN + FG_BLACK);

    // ---------- Introduction ----------
    printHeading("INTRODUCTION");
    printText("During a disaster, relief supplies such as food, water,");
    printText("medicine and shelter kits are limited, and many areas");
    printText("need them at once. This program keeps track of the");
    printText("available stock and distributes it to affected areas.");
    cout << "\n";
    printText(BOLD + string("With this system you can:") + RESET);
    printBullet("Request relief supplies for an area");
    printBullet("View the supplies currently in stock");
    printBullet("Add new supplies when donations arrive");
    printBullet("Check the overall distribution status");

    // ---------- Commands ----------
    printHeading("COMMANDS");
    printTableHeader();
    printCommand("help",         "Request relief supplies for an area", false);
    printCommand("resources",    "Print all supplies and quantities",   true);
    printCommand("update",          "Add more stock of a supply",          false);
    printCommand("status",       "Show summary of distributions",       true);
    printCommand("instructions", "Show this instructions screen",       false);
    printCommand("exit",         "Exit the program",                    true);

    // ---------- How to use ----------
    printHeading("HOW TO USE");
    printStep(1, "Type a command at the  >  prompt and press Enter.");
    printStep(2, "Answer the questions (supply, quantity, area).");
    printStep(3, "Quantities must be positive whole numbers.");
    printStep(4, "You cannot request more than what is in stock.");

    // ---------- Example ----------
    printHeading("EXAMPLE");
    printTerminalLine("",                                FG_GREEN);
    printTerminalLine("> help",                          FG_GREEN + BOLD);
    printTerminalLine("  Enter supply name : Water",     FG_WHITE);
    printTerminalLine("  Enter quantity    : 50",        FG_WHITE);
    printTerminalLine("  Enter area name   : Muzaffarabad", FG_WHITE);
    printTerminalLine("  Request approved!",             FG_YELLOW + BOLD);
    printTerminalLine("",                                FG_GREEN);

    // ---------- Footer ----------
    cout << "\n";
    printBar("Type 'instructions' at any time to see this screen again.",
             BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";
}


void printExitMessage(){
    cout << "\n\n";
    printBar("Thank you for using the Relief Supply Distribution System.",
             BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";
}

void invalidCommandMessage(){
    cout << "\n";
    printBar("Invalid command. Type 'instructions' to see the list of commands.",
             BG_YELLOW + FG_BLACK + BOLD);
    cout << "\n";
}