#include "./../include/output.h"
#include "./../include/inventory.h"
#include "./../include/utility.h"
#include "./../include/engine.h"
#include "../include/storage.h"
#include <iostream>
#include <string>
using namespace std;
enum Commands
{
    HISTORY = 7
    EXIT = 0,
    RESOURCES = 1,
    HELP = 2,
    STATUS = 3,
    UPDATE = 4,
    INSTRUCTIONS = 5,
    INVALID_COMMAND = -1
};

Commands tocommands(const string &command)
{
    if (command == "exit")
        return EXIT;
    else if (command == "resources")
        return RESOURCES;
    else if (command == "help")
        return HELP;
    else if (command == "status")
        return STATUS;
    else if (command == "update")
        return UPDATE;
    else if (command == "instructions")
        return INSTRUCTIONS;
    else if (command == "history" || command == "requests")
        return HISTORY;
    else
        return INVALID_COMMAND; // Default case
}

int main()
{
    printInstructions();
    string input;
    while (true)
    {
	  cout << "\n  " << FG_GREEN << BOLD << "Enter a command (or 'exit' to quit): " << RESET;
        getline(cin, input);
        string cleanInputStr = cleanInput(input);
        switch (tocommands(cleanInputStr))
        {
        case EXIT:
		if (confirmExit()) // If confirmExit is true
		{
		    printExitMessage();
		    return 0;
		}
            break;
        case RESOURCES:
            showInventory();
            break;
        case HELP:
            getHelp();
            break;
        case INSTRUCTIONS:
            printInstructions();
            break;
	  case STATUS:
		showStatus();
		break;
	  case UPDATE:
		addStock();
		break;
	  case HISTORY:
         showRequestHistorySummary();
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
