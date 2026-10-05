#include "./../include/engine.h"
#include "./../include/output.h"
#include "./../include/utility.h"
#include "./../include/storage.h"
#include <iostream>
#include <string>
using namespace std;

// ======================= Input limits ===========================
const int MIN_FAMILIES = 10;
const int MAX_FAMILIES = 200;
const int MIN_DISTANCE = 1;      // km
const int MAX_DISTANCE = 500;    // km
const int MAX_INJURED  = 1000;

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

void getHelp(){
    HelpRequest request = takeInputs();
    saveRequest(request);
    // TODO: calculate the need score and choose an action from 'request'
}
