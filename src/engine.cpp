#include "./../include/engine.h"
#include "./../include/output.h"
#include "./../include/utility.h"
#include "./../include/storage.h"
#include "./../include/inventory.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

// ======================= Input limits ===========================
const int MIN_FAMILIES = 10;
const int MAX_FAMILIES = 200;
const int MIN_DISTANCE = 1;      // km
const int MAX_DISTANCE = 500;    // km
const int MAX_INJURED = 1000;

// ======================= Relief packages ========================

// One row per package, in the same order as the Action enum,
// so PACKAGES[FULL_RELIEF] is the Full Relief row
const ReliefPackage PACKAGES[PACKAGE_COUNT] = {
    // name                   trucks  packs waterbottles foodpackets medicinekits
    { "A. Full Relief",          2,     50 ,    100,        50,            25 },
    { "B. Standard Relief",      1,     30 ,    60,         30,            15 },
    { "C. Emergency Kit Only",   1,     10 ,    20,         10,            5 },
    { "D. Defer / Monitor",      0,      0 ,     0,          0,             0 }
};

// True if there is enough of EVERY item
bool canAfford(Action action) {
    return trucks >= PACKAGES[action].trucksNeeded
	  && packs >= PACKAGES[action].packsNeeded
	  && waterBottles >= PACKAGES[action].waterBottlesNeeded
	  && foodPackets >= PACKAGES[action].foodPacketsNeeded
	  && medicineKits >= PACKAGES[action].medicineKitsNeeded;
}

// Takes everything the package needs out of stock.
// Refuses (returns false) instead of letting stock go below zero.
bool dispatchPackage(Action action) {
    if (!canAfford(action))
	  return false;

    trucks -= PACKAGES[action].trucksNeeded;
    packs -= PACKAGES[action].packsNeeded;
    waterBottles -= PACKAGES[action].waterBottlesNeeded;
    foodPackets -= PACKAGES[action].foodPacketsNeeded;
    medicineKits -= PACKAGES[action].medicineKitsNeeded;
    return true;
}

// ======================= Generic input helpers ==================

// Prints an error line in yellow so it stands out from normal text
void printInputError(string message) {
    cout << "    " << FG_YELLOW << BOLD << "! " << message << RESET << "\n";
}

// Asks for free text and repeats until the answer is not empty
string askText(string question) {
    string answer;
    while (true) {
	  cout << "    " << question << ": ";
	  getline(cin, answer);
	  if (cleanInput(answer) != "")
		return answer;
	  printInputError("This cannot be left empty.");
    }
}

// Asks for a whole number and repeats until it is between min and max.
// Converted by hand (digit by digit) so letters, signs or decimals are rejected.
int askNumber(string question, int min, int max) {
    string answer;
    while (true) {
	  cout << "    " << question << " (" << min << "-" << max << "): ";
	  getline(cin, answer);
	  answer = cleanInput(answer);

	  bool valid = (answer != "" && answer.length() <= 6);
	  int value = 0;
	  for (int i = 0; valid && i < (int)answer.length(); i++) {
		if (answer[i] < '0' || answer[i] > '9')
		    valid = false;
		else
		    value = value * 10 + (answer[i] - '0');
	  }

	  if (valid && value >= min && value <= max)
		return value;
	  printInputError("Please enter a whole number from " + to_string(min)
		+ " to " + to_string(max) + ".");
    }
}

// Asks the user to pick one of two answers; returns true for the first one.
// Typing just the first letter also works (e.g. "y" for "yes").
bool askTwoChoice(string question, string first, string second) {
    string answer;
    while (true) {
	  cout << "    " << question << " (" << first << "/" << second << "): ";
	  getline(cin, answer);
	  answer = cleanInput(answer);

	  if (answer == first || answer == first.substr(0, 1))
		return true;
	  if (answer == second || answer == second.substr(0, 1))
		return false;
	  printInputError("Please type '" + first + "' or '" + second + "'.");
    }
}

bool askYesNo(string question) {
    return askTwoChoice(question, "yes", "no");
}

// Asks for low / medium / high (or l / m / h)
Level askLevel(string question) {
    string answer;
    while (true) {
	  cout << "    " << question << " (low/medium/high): ";
	  getline(cin, answer);
	  answer = cleanInput(answer);

	  if (answer == "low" || answer == "l") return LOW;
	  if (answer == "medium" || answer == "m") return MEDIUM;
	  if (answer == "high" || answer == "h") return HIGH;
	  printInputError("Please type 'low', 'medium' or 'high'.");
    }
}

// ======================= Request input ==========================

HelpRequest takeInputs() {
    HelpRequest request;

    cout << "\n";
    printBar("New Relief Request", BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";

    request.areaName = askText("Area name");
    request.damageLevel = askLevel("Damage level");
    request.familiesAffected = askNumber("Families affected", MIN_FAMILIES, MAX_FAMILIES);

    request.routeOpen = askTwoChoice("Access route", "open", "damaged");
    if (request.routeOpen)
	  request.distanceKm = askNumber("Distance to the area in km", MIN_DISTANCE, MAX_DISTANCE);
    else
	  request.distanceKm = 0;

    request.waterScarcity = askLevel("Water scarcity");
    request.foodAvailability = askLevel("Food availability");
    request.injuredPeople = askNumber("Number of injured people", 0, MAX_INJURED);
    request.previousAid = askYesNo("Previous aid received");

    return request;
}

// ======================= Main function ==========================

int calculateNeedScore(const HelpRequest& request) {
    int score = 0;

    // Damage level
    score += (int)request.damageLevel * 3;

    // Families affected
    if (request.familiesAffected > 100)
	  score += 3;
    else if (request.familiesAffected > 50)
	  score += 2;
    else
	  score += 1;

    // Route status: maximum 3 points
    if (!request.routeOpen)
	  score += 3;
    else if (request.distanceKm > 100)
	  score += 2;
    else
	  score += 1;

    // Water scarcity
    score += (int)request.waterScarcity * 2;

    // Food availability
    score += (3 - (int)request.foodAvailability);

    // Injured people: maximum 4 points
    if (request.injuredPeople == 0)
	  score += 0;
    else if (request.injuredPeople <= 100)
	  score += 1;
    else if (request.injuredPeople <= 500)
	  score += 2;
    else
	  score += 4;

    // Previous aid received
    if (!request.previousAid)
	  score += 2;

    return score;
}

// ======================= Result display =========================
const int RES_MARGIN = 4;    // left indent of everything in the report
const int LABEL_W = 26;   // width of the "Label ......" part of a detail line
const int MAX_NEED_SCORE = 25; // highest score calculateNeedScore() can give - update if the formula changes
const int STOCK_NAME_W = 16;   // stock table: resource name column
const int STOCK_NUM_W = 11;   // stock table: each number column

// Colour for a low/medium/high answer. 'highIsBad' is false for food
// availability, because having a LOT of food is the good case.
string levelColour(Level level, bool highIsBad) {
    Level worst = highIsBad ? HIGH : LOW;
    if (level == worst)  return FG_RED;
    if (level == MEDIUM) return FG_YELLOW;
    return FG_GREEN;
}

// Same idea as actionColour, but as a background for the score meter
string actionBackground(Action action) {
    switch (action) {
    case FULL_RELIEF:     return BG_RED;
    case STANDARD_RELIEF: return BG_YELLOW;
    case EMERGENCY_KIT:   return BG_CYAN;
    default:              return BG_GREEN;
    }
}

// "1 truck" / "2 trucks" - adds an 's' unless the count is exactly one
string countOf(int count, string word) {
    return to_string(count) + " " + word + (count == 1 ? "" : "s");
}

// Adds "name (need X, have Y)" to the list, but only if we are short of it
void addShortage(string& list, string name, int need, int have) {
    if (have >= need)
	  return;
    if (list != "")
	  list += ", ";
    list += name + " (need " + to_string(need) + ", have " + to_string(have) + ")";
}

// Lists only the items that stop this package from being sent,
// e.g. "packs (need 50, have 20), water bottles (need 100, have 40)"
string missingItems(Action action) {
    string list = "";
    addShortage(list, "trucks", PACKAGES[action].trucksNeeded, trucks);
    addShortage(list, "packs", PACKAGES[action].packsNeeded, packs);
    addShortage(list, "water bottles", PACKAGES[action].waterBottlesNeeded, waterBottles);
    addShortage(list, "food packets", PACKAGES[action].foodPacketsNeeded, foodPackets);
    addShortage(list, "medicine kits", PACKAGES[action].medicineKitsNeeded, medicineKits);
    return list;
}

// Echo of everything the user typed, so they can check it at a glance
void printRequestSummary(const HelpRequest& request) {
    string route = request.routeOpen
	  ? "Open (" + to_string(request.distanceKm) + " km)"
	  : "Damaged";

    cardTop("REQUEST SUMMARY");
    cardPair("Damage level", levelToText(request.damageLevel), levelColour(request.damageLevel, true),
	  "Families affected", to_string(request.familiesAffected), FG_WHITE);
    cardPair("Access route", route, request.routeOpen ? FG_GREEN : FG_RED,
	  "Injured people", to_string(request.injuredPeople), FG_WHITE);
    cardPair("Water scarcity", levelToText(request.waterScarcity), levelColour(request.waterScarcity, true),
	  "Previous aid", yesNo(request.previousAid), request.previousAid ? FG_GREEN : FG_YELLOW);
    cardPair("Food availability", levelToText(request.foodAvailability), levelColour(request.foodAvailability, false),
	  "", "", "");
    cardBottom();
}

// "2 trucks, 50 packs, 100 water, 50 food, 25 medicine" - short version of what a package uses
string shortCost(Action action) {
    return countOf(PACKAGES[action].trucksNeeded, "truck") + ", "
	  + countOf(PACKAGES[action].packsNeeded, "pack") + ", "
	  + to_string(PACKAGES[action].waterBottlesNeeded) + " water, "
	  + to_string(PACKAGES[action].foodPacketsNeeded) + " food, "
	  + to_string(PACKAGES[action].medicineKitsNeeded) + " medicine";
}

// Text style for the package label (white on red, otherwise black)
string badgeStyle(Action action) {
    return actionBackground(action) + (action == FULL_RELIEF ? FG_WHITE : FG_BLACK) + BOLD;
}

void printNeedAssessment(int score, Action recommended) {
    cardTop("NEED ASSESSMENT");
    cardMeter("Need score", score, MAX_NEED_SCORE, actionBackground(recommended));
    cardLabelled("Package", " " + upperCase(PACKAGES[recommended].name) + " ", badgeStyle(recommended));
    if (recommended != DEFER)
	  cardLabelled("Needs", shortCost(recommended), FG_GRAY);
    cardBottom();
}

// One step of the progress line:  "● Requested"  or  "○ On the way"
void addStage(string& line, int& visible, const string& name, bool reached, const string& colour) {
    if (visible > 0) {
	  line += FG_GRAY + " " + LINE_H + " " + RESET;
	  visible += 3;
    }
    if (reached)
	  line += colour + BOLD + DOT_FULL + " " + name + RESET;
    else
	  line += FG_GRAY + DOT_EMPTY + " " + name + RESET;
    visible += 2 + (int)name.length();
}

// The progress line. The last two steps stay empty because this program does not track deliveries.
void cardProgress(bool served) {
    string line = "";
    int visible = 0;
    addStage(line, visible, "Requested", true, FG_GREEN);
    addStage(line, visible, "Processed", true, FG_GREEN);
    if (served)
	  addStage(line, visible, "Dispatched", true, FG_GREEN);
    else
	  addStage(line, visible, "Deferred", true, FG_RED);
    addStage(line, visible, "On the way", false, "");
    addStage(line, visible, "Reached", false, "");
    cardLine(line, visible);
}

// One row of the stock table: name, before, used, left
void stockRow(const string& name, int before, int left) {
    cout << rowStart() << cell(name, 16, ALIGN_LEFT, FG_WHITE)
	  << cell(to_string(before), 8, ALIGN_RIGHT, FG_WHITE)
	  << cell(to_string(before - left), 8, ALIGN_RIGHT, FG_YELLOW)
	  << cell(to_string(left), 8, ALIGN_RIGHT, (left == 0 ? FG_RED : FG_GREEN) + BOLD) << "\n";
}

void printStockTable(int trucksBefore, int packsBefore, int waterBefore, int foodBefore, int medicineBefore) {
    printRule('t', 16, 8, 8, 8);
    cout << rowStart() << cell("RESOURCE", 16, ALIGN_LEFT, FG_CYAN + BOLD)
	  << cell("BEFORE", 8, ALIGN_RIGHT, FG_CYAN + BOLD)
	  << cell("USED", 8, ALIGN_RIGHT, FG_CYAN + BOLD)
	  << cell("LEFT", 8, ALIGN_RIGHT, FG_CYAN + BOLD) << "\n";
    printRule('m', 16, 8, 8, 8);
    stockRow("Trucks", trucksBefore, trucks);
    stockRow("Supply packs", packsBefore, packs);
    stockRow("Water bottles", waterBefore, waterBottles);
    stockRow("Food packets", foodBefore, foodPackets);
    stockRow("Medicine kits", medicineBefore, medicineKits);
    printRule('b', 16, 8, 8, 8);
}

// ======================= Help command ===========================

void getHelp() {
    HelpRequest request = takeInputs();
    saveRequest(request);

    printHeading("REQUEST " + to_string(requestsServed + requestsDeferred + 1) + "  -  "
	  + upperCase(shorten(request.areaName, 30)));
    printRequestSummary(request);

    // ---------- Need assessment ----------
    int needScore = calculateNeedScore(request);
    Action relief = (needScore >= 20) ? FULL_RELIEF :
	  (needScore >= 12) ? STANDARD_RELIEF :
	  (needScore >= 8) ? EMERGENCY_KIT :
	  DEFER;
    Action recommended = relief;
    printNeedAssessment(needScore, recommended);

    // ---------- Dispatch ----------
    int trucksBefore = trucks;
    int packsBefore = packs;
    int waterBefore = waterBottles;
    int foodBefore = foodPackets;
    int medicineBefore = medicineKits;

    // Find what can really be sent: try the recommended package; if stock is short,
    // step down to the next smaller one
    while (relief != DEFER && !canAfford(relief)) {
	  relief = (relief == FULL_RELIEF) ? STANDARD_RELIEF :
		(relief == STANDARD_RELIEF) ? EMERGENCY_KIT :
		DEFER;
    }

    cardTop("DISPATCH STATUS");
    cardProgress(relief != DEFER);
    cardBlank();
    if (recommended == DEFER)
	  cardText("Score too low for a relief package.", FG_YELLOW);
    for (int a = recommended; a < relief; a++)      // the packages we had to skip
	  cardCheck(false, PACKAGES[a].name + "  - not enough " + missingItems((Action)a));
    if (relief != DEFER)
	  cardCheck(true, PACKAGES[relief].name + "  - sent");
    cardBottom();

    dispatchPackage(relief);
    if (relief == DEFER) requestsDeferred++;
    else                 requestsServed++;
    recordRequest(request.areaName, needScore, relief != DEFER, PACKAGES[relief].name);

    printHeading("STOCK");
    printStockTable(trucksBefore, packsBefore, waterBefore, foodBefore, medicineBefore);

    // ---------- Final result ----------
    cout << "\n";
    if (relief == DEFER) {
	  printBar("DEFERRED  /  MONITORING", BG_YELLOW + FG_BLACK + BOLD);
	  cout << "\n";
	  if (recommended == DEFER)
		printText("Reason: the need score is too low for a relief package right now.");
	  else
		printText("Reason: not enough stock left for any package.");
    }
    else {
	  printBar("DISPATCHED:  " + PACKAGES[relief].name, BG_GREEN + FG_BLACK + BOLD);
	  if (relief != recommended) {
		cout << "\n";
		printText("Note: downgraded from " + PACKAGES[recommended].name
		    + " because stock was short.");
	  }
    }
    cout << "\n";
    printLowStockWarnings();
}

// ======================= Admin password =========================
const string ADMIN_PASSWORD = "admin@123";   // password
const int MAX_PASSWORD_TRIES = 3;

// Asks for the admin password. Returns true if it is correct.
// User gets 3 tries, if wrong on each try then is locked out of this command.
bool askAdminPassword() {
    for (int tries = 1; tries <= MAX_PASSWORD_TRIES; tries++) {
	  cout << "    Admin password: ";
	  if (readPassword() == ADMIN_PASSWORD)
		return true;
	  printInputError("Wrong password (" + to_string(tries) + " of " + to_string(MAX_PASSWORD_TRIES) + ").");
    }
    cout << "\n";
    printBar("ACCESS DENIED", BG_RED + FG_WHITE + BOLD);
    return false;
}

// "1. Choose action:" - yellow number, cyan title
void printQuestionTitle(int number, const string& title) {
    cout << "    " << FG_YELLOW << BOLD << number << "." << RESET
	  << " " << FG_CYAN << BOLD << title << RESET << "\n";
}

// Green box that confirms the change
void printUpdated(const string& label, bool added, int amount, int previous, int total) {
    string text = label + " updated successfully!";
    string before = "Previous: ";
    string middle = added ? "   |   Added: " : "   |   Removed: ";
    string last = "   |   New total: ";
    string n1 = to_string(previous), n2 = to_string(amount), n3 = to_string(total);

    cardTop("UPDATED", 64, FG_GREEN, FG_GREEN + BOLD);
    cardLine(BG_GREEN + FG_BLACK + BOLD + " OK " + RESET + "  " + FG_GREEN + BOLD + text + RESET,
	  6 + (int)text.length());
    cardLine(FG_GRAY + before + RESET + BOLD + FG_WHITE + n1 + RESET
	  + FG_GRAY + middle + RESET + (added ? FG_GREEN : FG_YELLOW) + n2 + RESET
	  + FG_GRAY + last + RESET + FG_GREEN + n3 + RESET,
	  (int)(before.length() + n1.length() + middle.length() + n2.length()
		+ last.length() + n3.length()));
    cardBottom();
}

// Adds or removes stock by hand (donations, losses, corrections)
void addStock() {
    cout << "\n";
    if (!askAdminPassword()) return;

    cout << "\n";
    printBar("UPDATE STOCK", BG_BLUE + FG_WHITE + BOLD);
    printBar("Add new supplies or remove used stock", BG_CYAN + FG_BLACK);
    cout << "\n";

    printQuestionTitle(1, "Choose action:");
    printText("    1. Add stock");
    printText("    2. Remove stock");
    bool add = (askNumber("  Enter choice", 1, 2) == 1);

    cout << "\n";
    printQuestionTitle(2, "Select resource:");
    printText("     1. Trucks");
    printText("     2. Supply packs");
    printText("     3. Water bottles");
    printText("     4. Food packets");
    printText("     5. Medicine kits");
    int item = askNumber("  Enter choice", 1, 5);

    cout << "\n";
    printQuestionTitle(3, "Enter quantity:");
    int amount = askNumber(" Quantity", 1, 1000);

    int* stock = (item == 1) ? &trucks : (item == 2) ? &packs : (item == 3) ? &waterBottles : (item == 4) ? &foodPackets : &medicineKits;
    string label = (item == 1) ? "Trucks" : (item == 2) ? "Supply packs" : (item == 3) ? "Water bottles" : (item == 4) ? "Food packets" : "Medicine kits";

    int previous = *stock;
    if (!add && amount > previous) {
	  printInputError("Cannot remove more than you have (" + to_string(previous) + ")");
	  return;
    }
    *stock += add ? amount : -amount;

    cout << "\n";
    printUpdated(label, add, amount, previous, *stock);
    waitForEnter();
}

bool confirmExit() {
    printHeading("EXIT PROGRAM"); // Header Banner

    while (true) { // Keeps looping till the user gives an affirmation
	  cout << "    Are you sure you want to exit? (y/n): ";
	  string answer;
	  getline(cin, answer);
	  answer = cleanInput(answer); // Cleans up spaces and makes the letters lowercased

	  if (answer == "y" || answer == "yes")
		return true; // Close the program

	  if (answer == "n" || answer == "no")
		return false; // Stay in the program

	  printInputError("Please type 'y' or 'n'."); // Catches invalid answers/inputs
    }
}