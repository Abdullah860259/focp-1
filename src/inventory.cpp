#include "./../include/inventory.h"
#include "./../include/output.h"
#include "./../include/utility.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

// Starting stock
// Starting stock
const int START_TRUCKS = 20;
const int START_PACKS = 120;
const int START_WATER = 500;
const int START_FOOD = 300;
const int START_MEDICINE = 200;

int trucks = START_TRUCKS;
int packs = START_PACKS;
int waterBottles = START_WATER;
int foodPackets = START_FOOD;
int medicineKits = START_MEDICINE;

int requestsServed = 0;
int requestsDeferred = 0;
// ======================= Table layout ===========================
const int INV_MARGIN = 4;    // left indent of the table
const int ITEM_W     = 24;   // width of the item name column
const int QTY_W      = 10;   // width of the quantity column

// ======================= Main function ==========================

// ======================= Low stock ==============================
const int LOW_STOCK_PERCENT = 25;   // "low" = under this % of the starting stock
const int LEVEL_W = 10;             // width of the little level bar

// True if less than 20% of the starting stock is left
bool isLow(int qty, int start) {
    return qty * 100 / start < LOW_STOCK_PERCENT;
}

// One table row: name, in stock, start, level bar, status
static void printLevelRow(const string& item, int qty, int start) {
    int percent = qty * 100 / start;

    // Level bar: one filled square per 10 percent (at least one if anything is left)
    int filled = percent / 10;
    if (filled > LEVEL_W) filled = LEVEL_W;
    if (filled < 1 && qty > 0) filled = 1;
    string barColour = percent < LOW_STOCK_PERCENT ? "\033[101m" : percent < 50 ? "\033[103m" : "\033[102m";

    cout << rowStart() << cell(item, 15, ALIGN_LEFT, FG_WHITE)
	  << cell(to_string(qty), 8, ALIGN_RIGHT, FG_YELLOW + BOLD)
	  << cell(to_string(start), 6, ALIGN_RIGHT, FG_GRAY)
	  << barCell(filled, LEVEL_W, barColour)
	  << (isLow(qty, start) ? cell(" LOW ", 6, ALIGN_CENTER, BG_RED + FG_WHITE + BOLD)
		: cell("OK", 6, ALIGN_CENTER, FG_GREEN + BOLD))
	  << "\n";
}

// Prints a red warning line, but only if this item is low
void warnIfLow(const string& item, int qty, int start) {
    if (isLow(qty, start))
	  cout << "  " << FG_RED << BOLD << BLINK << "!! LOW STOCK: " << RESET
	  << FG_RED << BOLD << item << " (" << qty << " of " << start << " left)" << RESET << "\n";
}

void printLowStockWarnings() {
    warnIfLow("Trucks", trucks, START_TRUCKS);
    warnIfLow("Supply packs", packs, START_PACKS);
    warnIfLow("Water bottles", waterBottles, START_WATER);
    warnIfLow("Food packets", foodPackets, START_FOOD);
    warnIfLow("Medicine kits", medicineKits, START_MEDICINE);
}

// ======================= Resources screen =======================

void showInventory() {
    printHeading("INVENTORY");
    printSubtitle("Current stock levels, shown against the starting stock");
    cout << "\n";

    printRule('t', 15, 8, 6, LEVEL_W, 6);
    cout << rowStart() << cell("RESOURCE", 15, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("IN STOCK", 8, ALIGN_RIGHT, FG_CYAN + BOLD)
	  << cell("START", 6, ALIGN_RIGHT, FG_CYAN + BOLD)
	  << cell("LEVEL", LEVEL_W, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("STATUS", 6, ALIGN_LEFT, FG_CYAN + BOLD) << "\n";
    printRule('m', 15, 8, 6, LEVEL_W, 6);
    printLevelRow("Trucks", trucks, START_TRUCKS);
    printLevelRow("Supply packs", packs, START_PACKS);
    printLevelRow("Water bottles", waterBottles, START_WATER);
    printLevelRow("Food packets", foodPackets, START_FOOD);
    printLevelRow("Medicine kits", medicineKits, START_MEDICINE);
    printRule('b', 15, 8, 6, LEVEL_W, 6);

    cout << "\n";
    printLowStockWarnings();
    printSubtitle("LOW means under " + to_string(LOW_STOCK_PERCENT) + "% of the starting stock.");
    waitForEnter();
}

// ======================= Status screen ==========================

// The finished table rows of the status screen, one row per request
string historyRows = "";

// Keeps one table row for a request. Call it AFTER requestsServed / requestsDeferred
// have been increased, because the row number is their total.
void recordRequest(const string & area, int score, bool served, const string & package) {
    string badge = served ? cell(" SERVED ", 10, ALIGN_CENTER, BG_GREEN + FG_BLACK + BOLD)
	  : cell(" DEFERRED ", 10, ALIGN_CENTER, BG_RED + FG_WHITE + BOLD);

    historyRows += rowStart()
	  + cell(to_string(requestsServed + requestsDeferred), 3, ALIGN_LEFT, FG_WHITE)
	  + cell(shorten(area, 19), 19, ALIGN_LEFT, FG_WHITE)
	  + cell(to_string(score), 5, ALIGN_RIGHT, FG_WHITE)
	  + badge
	  + cell(package, 21, ALIGN_LEFT, FG_WHITE) + "\n";
}

void showStatus() {
    int processed = requestsServed + requestsDeferred;

    printHeading("DISTRIBUTION SUMMARY");
    printSubtitle("Overview of processed requests and distribution results");
    cout << "\n  " << FG_CYAN << BOLD << DOT_FULL << RESET << " Requests processed: " << BOLD << FG_WHITE << processed << RESET
	  << "     " << FG_GREEN << BOLD << DOT_FULL << RESET << " Served: " << FG_GREEN << requestsServed << RESET
	  << "     " << FG_RED << BOLD << DOT_FULL << RESET << " Deferred: " << FG_RED << requestsDeferred << RESET
	  << "\n\n";

    if (processed == 0) {
	  printSubtitle("No requests have been processed yet. Type 'help' to make one.");
	  cout << "\n";
	  return;
    }

    printRule('t', 3, 19, 5, 10, 21);
    cout << rowStart() << cell("ID", 3, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("AREA", 19, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("SCORE", 5, ALIGN_RIGHT, FG_CYAN + BOLD)
	  << cell("STATUS", 10, ALIGN_CENTER, FG_CYAN + BOLD)
	  << cell("PACKAGE", 21, ALIGN_LEFT, FG_CYAN + BOLD) << "\n";
    printRule('m', 3, 19, 5, 10, 21);
    cout << historyRows;
    printRule('b', 3, 19, 5, 10, 21);

    cout << "\n  " << FG_YELLOW << BOLD << "Status legend:" << RESET << "\n"
	  << "  " << FG_GREEN << BOLD << "  " << DOT_FULL << " SERVED    " << RESET << FG_GRAY << "- package dispatched" << RESET << "\n"
	  << "  " << FG_RED << BOLD << "  " << DOT_FULL << " DEFERRED  " << RESET << FG_GRAY << "- score too low, or not enough stock" << RESET << "\n";
}