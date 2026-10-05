#pragma once

// Current stock, shared by every file that includes this header.
// The real variables (with their starting values) live in inventory.cpp.
extern int trucks;
extern int packs;
extern int waterBottles;
extern int foodPackets;
extern int medicineKits;

void showInventory();
