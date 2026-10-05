// write hello world in blue color

#include <iostream>
using namespace std;

int main() {
    cout << "\033[94mHello, World!\033[0m" << endl; // 94 is the ANSI code for blue
    return 0;
}