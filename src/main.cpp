// =================================================================
//  main.cpp - Relief Supply Distribution System
//
//  Program entry point. Shows the instructions screen, then keeps
//  reading commands from the user and sends each one to the right
//  function until the user exits.
// =================================================================

#include "./../include/output.h"
#include "./../include/inventory.h"
#include "./../include/utility.h"
#include "./../include/engine.h"
#include "../include/storage.h"
#include <iostream>
#include <string>
using namespace std;

// Every command the user can type, as a named number.
// The switch in main() uses these names instead of comparing strings everywhere.
enum Commands
{
    EXIT = 0,
    RESOURCES = 1,
    HELP = 2,
    STATUS = 3,
    UPDATE = 4,
    INSTRUCTIONS = 5,
    HISTORY = 7,
    INVALID_COMMAND = -1
};

// Turns the typed text into a Commands value.
// The text has already been cleaned (lowercase, no spaces) by cleanInput().
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
    printInstructions();     // welcome screen with the list of commands
    string input;

    // Main command loop: runs until the user confirms 'exit'
    while (true)
    {
	  cout << "\n  " << FG_GREEN << BOLD << "Enter a command (or 'exit' to quit): " << RESET;
        getline(cin, input);                          // read the whole line, spaces included
        string cleanInputStr = cleanInput(input);     // "  Help " -> "help"

        // Run the function that belongs to the command
        switch (tocommands(cleanInputStr))
        {
        case EXIT:
		if (confirmExit()) // only quit if the user answers yes
		{
		    printExitMessage();
		    return 0;
		}
            break;
        case RESOURCES:
            showInventory();          // stock table with low-stock warnings
            break;
        case HELP:
            getHelp();                // new relief request
            break;
        case INSTRUCTIONS:
            printInstructions();      // show the welcome screen again
            break;
	  case STATUS:
		showStatus();             // served / deferred summary
		break;
	  case UPDATE:
		addStock();               // admin: add or remove stock
		break;
	  case HISTORY:
         showRequestHistorySummary();   // where saved requests are stored
         break;
        case INVALID_COMMAND:
            invalidCommandMessage();  // unknown command
            break;
        default:
            break;
        }
    }
    return 0;
}
