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
const int MAX_INJURED  = 1000;

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

// True if there are enough trucks AND packs left to send this package
bool canAfford(Action action){
    return trucks >= PACKAGES[action].trucksNeeded
        && packs  >= PACKAGES[action].packsNeeded
        && waterBottles >= PACKAGES[action].waterBottlesNeeded
        && foodPackets >= PACKAGES[action].foodPacketsNeeded
        && medicineKits >= PACKAGES[action].medicineKitsNeeded;
}

// Takes everything the package needs out of stock.
// Refuses (returns false) instead of letting stock go below zero.
bool dispatchPackage(Action action){
    if (!canAfford(action))
        return false;

    trucks       -= PACKAGES[action].trucksNeeded;
    packs        -= PACKAGES[action].packsNeeded;
    waterBottles -= PACKAGES[action].waterBottlesNeeded;
    foodPackets  -= PACKAGES[action].foodPacketsNeeded;
    medicineKits -= PACKAGES[action].medicineKitsNeeded;
    return true;
}

// ======================= Generic input helpers ==================

// Prints an error line in yellow so it stands out from normal text
void printInputError(string message){
    cout << "    " << FG_YELLOW << BOLD << "! " << message << RESET << "\n";
}

// Asks for free text and repeats until the answer is not empty
string askText(string question){
    string answer;
    while (true){
        cout << "    " << question << ": ";
        getline(cin, answer);
        if (cleanInput(answer) != "")
            return answer;
        printInputError("This cannot be empty.");
    }
}

// Asks for a whole number and repeats until it is between min and max.
// Converted by hand (digit by digit) so letters, signs or decimals are rejected.
int askNumber(string question, int min, int max){
    string answer;
    while (true){
        cout << "    " << question << " (" << min << "-" << max << "): ";
        getline(cin, answer);
        answer = cleanInput(answer);

        bool valid = (answer != "" && answer.length() <= 6);
        int value = 0;
        for (int i = 0; valid && i < (int)answer.length(); i++){
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
bool askTwoChoice(string question, string first, string second){
    string answer;
    while (true){
        cout << "    " << question << " (" << first << "/" << second << "): ";
        getline(cin, answer);
        answer = cleanInput(answer);

        if (answer == first  || answer == first.substr(0, 1))
            return true;
        if (answer == second || answer == second.substr(0, 1))
            return false;
        printInputError("Please type '" + first + "' or '" + second + "'.");
    }
}

bool askYesNo(string question){
    return askTwoChoice(question, "yes", "no");
}

// Asks for low / medium / high (or l / m / h)
Level askLevel(string question){
    string answer;
    while (true){
        cout << "    " << question << " (low/medium/high): ";
        getline(cin, answer);
        answer = cleanInput(answer);

        if (answer == "low"    || answer == "l") return LOW;
        if (answer == "medium" || answer == "m") return MEDIUM;
        if (answer == "high"   || answer == "h") return HIGH;
        printInputError("Please type 'low', 'medium' or 'high'.");
    }
}

// ======================= Request input ==========================

HelpRequest takeInputs(){
    HelpRequest request;

    cout << "\n";
    printBar("New Relief Request", BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";

    request.areaName         = askText("Area name");
    request.damageLevel      = askLevel("Damage level");
    request.familiesAffected = askNumber("Families affected", MIN_FAMILIES, MAX_FAMILIES);

    request.routeOpen = askTwoChoice("Access route", "open", "damaged");
    if (request.routeOpen)
        request.distanceKm = askNumber("Distance to the area in km", MIN_DISTANCE, MAX_DISTANCE);
    else
        request.distanceKm = 0;

    request.waterScarcity    = askLevel("Water scarcity");
    request.foodAvailability = askLevel("Food availability");
    request.injuredPeople    = askNumber("Number of injured people", 0, MAX_INJURED);
    request.previousAid      = askYesNo("Previous aid received");

    return request;
}

// ======================= Main function ==========================

int calculateNeedScore(const HelpRequest &request){
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

    // Route status
    if (!request.routeOpen)
        score += 3;
    else if (request.distanceKm > 100)
        score += 2;
    else
        score += 1;

    // Water scarcity
    score += (int)request.waterScarcity * 2;

    // Food availability
    score += (int)(2 - request.foodAvailability) * 2;


    // Injured people
    if (request.injuredPeople > 500)
        score += 3;
    else if (request.injuredPeople > 100)
        score += 2;
    else
        score += 1;

    // Previous aid received
    if (!request.previousAid)
        score += 2;

    return score;
}

// ======================= Result display =========================
const int RES_MARGIN   = 4;    // left indent of everything in the report
const int LABEL_W      = 26;   // width of the "Label ......" part of a detail line
const int MAX_NEED_SCORE = 25; // highest score calculateNeedScore() can give - update if the formula changes
const int STOCK_NAME_W = 16;   // stock table: resource name column
const int STOCK_NUM_W  = 11;   // stock table: each number column

// One "Label ........ value" line; the dots are made with setfill('.')
void printDetail(string label, string value, string valueColour){
    cout << setw(RES_MARGIN) << "" << left << setfill('.') << setw(LABEL_W) << label + " "
         << setfill(' ') << right << " " << valueColour << BOLD << value << RESET << "\n";
}

// Colour for a low/medium/high answer. 'highIsBad' is false for food
// availability, because having a LOT of food is the good case.
string levelColour(Level level, bool highIsBad){
    Level worst = highIsBad ? HIGH : LOW;
    if (level == worst)  return FG_RED;
    if (level == MEDIUM) return FG_YELLOW;
    return FG_GREEN;
}

// Colour that goes with each package: the bigger the package, the more urgent
string actionColour(Action action){
    switch (action){
        case FULL_RELIEF:     return FG_RED;
        case STANDARD_RELIEF: return FG_YELLOW;
        case EMERGENCY_KIT:   return FG_CYAN;
        default:              return FG_GREEN;
    }
}

// Same idea as actionColour, but as a background for the score meter
string actionBackground(Action action){
    switch (action){
        case FULL_RELIEF:     return BG_RED;
        case STANDARD_RELIEF: return BG_YELLOW;
        case EMERGENCY_KIT:   return BG_CYAN;
        default:              return BG_GREEN;
    }
}

// "1 truck" / "2 trucks" - adds an 's' unless the count is exactly one
string countOf(int count, string word){
    return to_string(count) + " " + word + (count == 1 ? "" : "s");
}

// "2 trucks, 50 packs" - what a package costs
string packageCost(Action action){
    return countOf(PACKAGES[action].trucksNeeded, "truck") + ", "
         + countOf(PACKAGES[action].packsNeeded,  "pack") + ", "
         + countOf(PACKAGES[action].waterBottlesNeeded, "water bottle") + ", "
         + countOf(PACKAGES[action].foodPacketsNeeded, "food packet") + ", "
         + countOf(PACKAGES[action].medicineKitsNeeded, "medicine kit");
}

// Adds "name (need X, have Y)" to the list, but only if we are short of it
void addShortage(string &list, string name, int need, int have){
    if (have >= need)
        return;
    if (list != "")
        list += ", ";
    list += name + " (need " + to_string(need) + ", have " + to_string(have) + ")";
}

// Lists only the items that stop this package from being sent,
// e.g. "packs (need 50, have 20), water bottles (need 100, have 40)"
string missingItems(Action action){
    string list = "";
    addShortage(list, "trucks",        PACKAGES[action].trucksNeeded,       trucks);
    addShortage(list, "packs",         PACKAGES[action].packsNeeded,        packs);
    addShortage(list, "water bottles", PACKAGES[action].waterBottlesNeeded, waterBottles);
    addShortage(list, "food packets",  PACKAGES[action].foodPacketsNeeded,  foodPackets);
    addShortage(list, "medicine kits", PACKAGES[action].medicineKitsNeeded, medicineKits);
    return list;
}

// Echo of everything the user typed, so they can check it at a glance
void printRequestSummary(const HelpRequest &request){
    printHeading("REQUEST SUMMARY");

    string route = request.routeOpen
                 ? "Open (" + to_string(request.distanceKm) + " km)"
                 : "Damaged";

    printDetail("Area",              request.areaName,                      FG_WHITE);
    printDetail("Damage level",      levelToText(request.damageLevel),      levelColour(request.damageLevel, true));
    printDetail("Families affected", to_string(request.familiesAffected),   FG_WHITE);
    printDetail("Access route",      route,                                 request.routeOpen ? FG_GREEN : FG_RED);
    printDetail("Water scarcity",    levelToText(request.waterScarcity),    levelColour(request.waterScarcity, true));
    printDetail("Food availability", levelToText(request.foodAvailability), levelColour(request.foodAvailability, false));
    printDetail("Injured people",    to_string(request.injuredPeople),      FG_WHITE);
    printDetail("Previous aid",      yesNo(request.previousAid),            request.previousAid ? FG_GREEN : FG_YELLOW);
}

// Coloured bar showing the score out of MAX_NEED_SCORE, e.g.  [######------]  12 / 17
// Each point is two coloured spaces; unearned points are gray.
void printScoreMeter(int score, Action recommended){
    cout << setw(RES_MARGIN) << "" << left << setw(LABEL_W) << "Need score" << right << " ";
    for (int point = 1; point <= MAX_NEED_SCORE; point++){
        if (point <= score)
            cout << actionBackground(recommended) << "  ";
        else
            cout << BG_GRAY << "  ";
    }
    cout << RESET << "  " << actionColour(recommended) << BOLD << score << RESET
         << " / " << MAX_NEED_SCORE << "\n";
}

// One line of the dispatch log:  [ OK ] text  or  [ NO ] text
void printCheck(bool ok, string text){
    cout << setw(RES_MARGIN) << "" << BOLD
         << (ok ? BG_GREEN + FG_BLACK + " OK " : BG_RED + FG_WHITE + " NO ")
          << "  " << text << RESET << "\n";
}

// Header row of the stock table
void printStockHeader(){
    cout << setw(RES_MARGIN) << "" << BG_BLUE << FG_WHITE << BOLD
         << "  " << left  << setw(STOCK_NAME_W) << "RESOURCE"
         << right << setw(STOCK_NUM_W) << "BEFORE"
         << setw(STOCK_NUM_W) << "USED"
         << setw(STOCK_NUM_W) << "REMAINING" << "  "
         << RESET << "\n";
}

// One row of the stock table; remaining stock turns red when it hits zero
void printStockRow(string name, int before, int used, int remaining, bool shaded){
    string rowBg = shaded ? BG_GRAY : BG_BLACK;
    string remainingColour = (remaining == 0) ? FG_RED : FG_GREEN;

    cout << setw(RES_MARGIN) << "" << rowBg << FG_WHITE
         << "  " << left  << setw(STOCK_NAME_W) << name
         << right << setw(STOCK_NUM_W) << before
         << FG_YELLOW << setw(STOCK_NUM_W) << used
         << remainingColour << BOLD << setw(STOCK_NUM_W) << remaining << "  "
         << RESET << "\n";
}

// ======================= Help command ===========================

void getHelp(){
    HelpRequest request = takeInputs();
    saveRequest(request);
    printRequestSummary(request);

    // ---------- Need assessment ----------
    int needScore = calculateNeedScore(request);
    Action relief = (needScore >= 20) ? FULL_RELIEF :
                    (needScore >= 12)  ? STANDARD_RELIEF :
                    (needScore >= 8)  ? EMERGENCY_KIT :
                                       DEFER;
    Action recommended = relief;

    printHeading("NEED ASSESSMENT");
    printScoreMeter(needScore, recommended);
    printDetail("Recommended package", PACKAGES[recommended].name, actionColour(recommended));
    if (recommended != DEFER)
        printDetail("Package needs", packageCost(recommended), FG_WHITE);

    // ---------- Dispatch ----------
    printHeading("DISPATCH");
    int trucksBefore = trucks;
    int packsBefore  = packs;
    int waterBefore  = waterBottles;
    int foodBefore   = foodPackets;
    int medicineBefore = medicineKits;

    // Try the recommended package; if stock is short, step down to the next smaller one
    while (relief != DEFER && !canAfford(relief)){
        printCheck(false, PACKAGES[relief].name + "  - not enough " + missingItems(relief));
        relief = (relief == FULL_RELIEF)     ? STANDARD_RELIEF :
                 (relief == STANDARD_RELIEF) ? EMERGENCY_KIT :
                                               DEFER;
    }
    dispatchPackage(relief);
    if (relief != DEFER)
        printCheck(true, PACKAGES[relief].name + "  - " + packageCost(relief) + " sent");

    cout << "\n";
    printStockHeader();
    printStockRow("Trucks",       trucksBefore, trucksBefore - trucks, trucks, false);
    printStockRow("Supply packs", packsBefore,  packsBefore  - packs,  packs,  true);
    printStockRow("Water bottles", waterBefore, waterBefore - waterBottles, waterBottles, false);
    printStockRow("Food packets",  foodBefore,  foodBefore  - foodPackets,  foodPackets,  true);
    printStockRow("Medicine kits", medicineBefore, medicineBefore - medicineKits, medicineKits, false); 

    // ---------- Final result ----------
    cout << "\n";
    if (relief == DEFER){
        printBar("DEFERRED  /  MONITORING", BG_YELLOW + FG_BLACK + BOLD);
        cout << "\n";
        if (recommended == DEFER)
            printText("Reason: the need score is too low for a relief package right now.");
        else
            printText("Reason: not enough trucks or supply packs left for any package.");
    }
    else{
        printBar("DISPATCHED:  " + PACKAGES[relief].name, BG_GREEN + FG_BLACK + BOLD);
        if (relief != recommended){
            cout << "\n";
            printText("Note: downgraded from " + PACKAGES[recommended].name
                      + " because stock was short.");
        }
    }
    cout << "\n";
}
