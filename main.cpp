#include <iostream>
#include "TuringCurses.hpp"

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FILE" << std::endl;
        return 1;
    }
    TuringCurses curses;
    if (curses.addStates(argv[1]))
        return 1;
    curses.initCurses();
    curses.main();
}
