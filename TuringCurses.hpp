#ifndef TURING_CURSES_HPP
#define TURING_CURSES_HPP

#include "TuringMachine.hpp"

class TuringCurses {
    TuringMachine machine_;
    WINDOW *stdscr_, *status_;
    int height_, width_;

    void drawScreen();
    void readInput();
    void updateSize();
    void writeStatus(const char* message);

public:
    TuringCurses();
    ~TuringCurses();

    void initCurses();

    bool addStates(const char* filename);

    int main();
};

#endif /* TURING_CURSES_HPP */
