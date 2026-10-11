// inventory.h - shared stock variables and the inventory / status screens.
// Definitions are in src/inventory.cpp.
#pragma once
#include <string>

// Current stock, shared by every file that includes this header.
// The real variables (with their starting values) live in inventory.cpp.
extern int trucks;
extern int packs;
extern int waterBottles;
extern int foodPackets;
extern int medicineKits;
// Request counters for the status screen
extern int requestsServed;
extern int requestsDeferred;

void showStatus();       // the "status" command
// Adds one row to the status table after a request is handled
void recordRequest(const std::string & area, int score, bool served, const std::string & package);
void showInventory();    // the "resources" command
void showSummaryStats();
void printLowStockWarnings();     // prints nothing if no item is low
