#include "./../include/storage.h"
#include "./../include/output.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
using namespace std;

#ifdef _WIN32
    #include <direct.h>      // _mkdir
#else
    #include <sys/stat.h>    // mkdir
#endif

// ======================= File settings ==========================
const string DATA_FOLDER   = "data";
const string REQUESTS_FILE = DATA_FOLDER + "/requests.csv";

// ======================= Helpers ================================

// Creates the data folder; does nothing if it already exists
void makeDataFolder(){
#ifdef _WIN32
    _mkdir(DATA_FOLDER.c_str());
#else
    mkdir(DATA_FOLDER.c_str(), 0755);
#endif
}

// True if the file is already there (so we don't write the header twice)
bool fileExists(string path){
    ifstream file(path);
    return file.good();
}

string levelToText(Level level){
    switch (level){
        case LOW:    return "Low";
        case MEDIUM: return "Medium";
        case HIGH:   return "High";
    }
    return "Unknown";
}

string yesNo(bool value){
    return value ? "Yes" : "No";
}

// Wraps text in quotes so commas inside it don't split the column.
// A quote inside the text is written twice ("") - that is the CSV rule.
string csvText(string text){
    string result = "\"";
    for (int i = 0; i < (int)text.length(); i++){
        if (text[i] == '"')
            result += "\"\"";
        else
            result += text[i];
    }
    return result + "\"";
}

// Current date and time, e.g. 2026-10-05 14:30:12
string currentTime(){
    time_t now = time(0);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

// ======================= Main function ==========================

// Adds one request as a new line at the end of data/requests.csv
void saveRequest(const HelpRequest &request){
    makeDataFolder();
    bool isNewFile = !fileExists(REQUESTS_FILE);

    ofstream file(REQUESTS_FILE, ios::app);    // ios::app = add to the end, keep old lines
    if (!file){
        cout << "    " << FG_YELLOW << BOLD << "! Could not open " << REQUESTS_FILE
             << ", request was not saved." << RESET << "\n";
        return;
    }

    if (isNewFile)
        file << "Time,Area,Damage Level,Families Affected,Route,Distance (km),"
             << "Water Scarcity,Food Availability,Injured People,Previous Aid\n";

    file << currentTime()                         << ","
         << csvText(request.areaName)             << ","
         << levelToText(request.damageLevel)      << ","
         << request.familiesAffected              << ","
         << (request.routeOpen ? "Open" : "Damaged") << ","
         << request.distanceKm                    << ","
         << levelToText(request.waterScarcity)    << ","
         << levelToText(request.foodAvailability) << ","
         << request.injuredPeople                 << ","
         << yesNo(request.previousAid)            << "\n";

    cout << "\n    " << FG_GREEN << BOLD << "Request saved to " << REQUESTS_FILE
         << RESET << "\n";
}
void showRequestHistorySummary() {
    cout << "\n\n";
    printBar("Relief Request Statistics", BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";
    printBullet("Total Requests Processed: Saved to " + REQUESTS_FILE);
    cout << "\n";
}
