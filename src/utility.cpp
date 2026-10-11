// =================================================================
//  utility.cpp - small helpers used all over the program
//
//  Cleaning typed input, changing text, reading a hidden password
//  and pausing until Enter is pressed.
// =================================================================

#include "./../include/utility.h"
#include "./../include/output.h"

#include <string>
#include <cctype>
#include <iostream>
using namespace std;

// conio.h gives _getch(), which reads one key without showing it (Windows only)
#ifdef _WIN32
#include <conio.h>      // _getch
#endif

// Removes all spaces and makes every letter lowercase, so "  Get Help " becomes "gethelp".
// This lets the user type commands and answers in any capitalisation.
string cleanInput(const string& input) {
    string cleaned;
    for (char c : input) {
	  if (!isspace(c))  // Ignore whitespace characters
		cleaned += tolower(c);  // Convert to lowercase
    }
    return cleaned;
}

// "full relief" -> "FULL RELIEF"
string upperCase(const string& text) {
    string result = text;
    for (char& c : result)
	  c = (char)toupper((unsigned char)c);
    return result;
}

// "Muzaffarabad City Centre" with maxLength 12 -> "Muzaffara..."
string shorten(const string& text, int maxLength) {
    if ((int)text.length() <= maxLength)
	  return text;
    return text.substr(0, maxLength - 3) + "...";
}

// Reads a password. On Windows every key shows as * and Backspace works.
// On other systems it simply reads a normal line.
string readPassword() {
#ifdef _WIN32
    string password;
    while (true) {
	  int key = _getch();
	  if (key == '\r' || key == '\n')
		break;
	  if (key == 0 || key == 224) {        // arrow / function keys send two codes
		_getch();
	  }
	  else if (key == 8) {               // Backspace
		if (!password.empty()) {
		    password.pop_back();
		    cout << "\b \b";
		}
	  }
	  else if (key >= 32) {
		password += (char)key;
		cout << '*';
	  }
    }
    cout << "\n";
    return password;
#else
    string line;
    getline(cin, line);
    return line;
#endif
}

// Pauses until the user presses Enter, so a screen does not scroll away straight away
void waitForEnter() {
    cout << "\n    " << FG_GRAY << "Press Enter to continue..." << RESET;
    string ignored;
    getline(cin, ignored);
}