#pragma once
#include <string>

// Current stock, shared by every file that includes this header.
// The real variables (with their starting values) live in inventory.cpp.
extern int trucks;
extern int packs;
extern int waterBottles;
extern int foodPackets;
extern int medicineKits;
extern int requestsServed;
extern int requestsDeferred;

void showStatus();
void recordRequest(const std::string & area, int score, bool served, const std::string & package);
void showInventory();
void printLowStockWarnings();     // prints nothing if no item is low