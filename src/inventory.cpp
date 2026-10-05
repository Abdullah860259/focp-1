#include "./../include/inventory.h"
#include "./../include/output.h"
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

int trucks = 20;
int packs = 120;
int waterBottles = 500;
int foodPackets = 300;
int medicineKits = 200;

// ======================= Table layout ===========================
const int INV_MARGIN = 4;    // left indent of the table
const int ITEM_W     = 24;   // width of the item name column
const int QTY_W      = 10;   // width of the quantity column

// Header row of the inventory table
void printInventoryHeader(){
    cout << setw(INV_MARGIN) << "" << BG_BLUE << FG_WHITE << BOLD
         << "  " << left  << setw(ITEM_W) << "ITEM"
         << "  " << right << setw(QTY_W)  << "QUANTITY" << "  "
         << RESET << "\n";
}

// One row of the inventory table; 'shaded' gives zebra striping
void printInventoryRow(string item, int qty, bool shaded){
    string rowBg = shaded ? BG_GRAY : BG_BLACK;

    cout << setw(INV_MARGIN) << "" << rowBg
         << FG_WHITE  << "  " << left  << setw(ITEM_W) << item
         << FG_YELLOW << BOLD << "  " << right << setw(QTY_W) << qty << "  "
         << RESET << "\n";
}

// ======================= Main function ==========================

void showInventory(){
    cout << "\n\n";
    printBar("Current Inventory Status", BG_BLUE + FG_WHITE + BOLD);
    cout << "\n";

    printInventoryHeader();
    printInventoryRow("Trucks",        trucks, false);
    printInventoryRow("Supply packs",  packs,  true);
    printInventoryRow("Water bottles",  waterBottles,    false);
    printInventoryRow("Food packets",   foodPackets,    true);
    printInventoryRow("Medicine kits",  medicineKits,    false);
    cout << "\n";
}
