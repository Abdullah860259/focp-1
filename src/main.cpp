#include "output.h"
#include <iostream>
using namespace std;

int main(){
    printInstructions();
    while(true){
        cout << "\n\nPress ENTER to continue...";
        cin.ignore();
    }
    return 0;
}