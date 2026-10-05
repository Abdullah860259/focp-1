#include "./../include/utility.h"

#include <string>
#include <cctype>
using namespace std;

string cleanInput(const string &input){
    string cleaned;
    for (char c : input){
        if (!isspace(c))  // Ignore whitespace characters
            cleaned += tolower(c);  // Convert to lowercase
    }
    return cleaned;
}