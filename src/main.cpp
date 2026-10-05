#include "./../include/output.h"
#include "./../include/inventory.h"
#include "./../include/utility.h"
#include "./../include/engine.h"
#include <iostream>
#include <string>
using namespace std;

enum Commands
{
    EXIT = 0,
    SHOW_INVENTORY = 1,
    GET_HELP = 2,
    REMOVE_ITEM = 3,
    UPDATE_ITEM = 4,
    INSTRUCTIONS = 5,
    INVALID_COMMAND = -1
};

Commands tocommands(const string &command)
{
    if (command == "exit")
        return EXIT;
    else if (command == "show_inventory")
        return SHOW_INVENTORY;
    else if (command == "get_help")
        return GET_HELP;
    else if (command == "remove_item")
        return REMOVE_ITEM;
    else if (command == "update_item")
        return UPDATE_ITEM;
    else if (command == "instructions")
        return INSTRUCTIONS;
    else
        return INVALID_COMMAND; // Default case
}

int main()
{
    printInstructions();
    string input;
    while (true)
    {
        cout << "\n\nEnter a command (or 'exit' to quit): ";
        getline(cin, input);
        string cleanInputStr = cleanInput(input);
        cout << "You entered: " << input << endl;
        switch (tocommands(cleanInputStr))
        {
        case EXIT:
            printExitMessage();
            return 0;
            break;
        case SHOW_INVENTORY:
            showInventory();
            break;
        case GET_HELP:
            getHelp();
            break;
        case REMOVE_ITEM:
            cout << "Remove item functionality not implemented yet." << endl;
            break;
        case UPDATE_ITEM:
            cout << "Update item functionality not implemented yet." << endl;
            break;
        case INSTRUCTIONS:
            printInstructions();
            break;
        case INVALID_COMMAND:
            invalidCommandMessage();
            break;
        default:
            break;
        }
    }
    return 0;
}