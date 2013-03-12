#include <curses.h>
#include "Tape.hpp"

void printTape(Tape& tape, WINDOW *win)
{
    int y, x;
    getmaxyx(win, y, x);
    move(0, 0);
    addch('[');
    tape.print(x - 2);
    addch(']');
    move(0, x / 2);
}

int main(int argc, const char* argv[])
{
    Tape tape;
    WINDOW *win = initscr();
    keypad(win, TRUE);
    int c = 0;
    printTape(tape, win);
    do {
        if (c >= 0x20 && c < 0x7F)
            tape.writeHead(c);
        else if (c == KEY_LEFT)
            tape.moveLeft();
        else if (c == KEY_RIGHT)
            tape.moveRight();
        printTape(tape, win);
    } while ((c = getch()) != '\n');
    endwin();
}
