#include <sstream>
#include "TuringMachine.hpp"

TuringMachine::TuringMachine() : stopped_(false) {}

StateParser& TuringMachine::parser()
{
    return register_.parser();
}

void TuringMachine::write(const char* str)
{
    int n = 0;
    tape_.clear();
    while (*str) {
        tape_.writeHead(*str);
        tape_.moveRight();
        ++n;
        ++str;
    }
    while (n--)
        tape_.moveLeft();
    stopped_ = false;
    register_.reset();
}

int TuringMachine::step()
{
    int r = register_.handle(tape_.readHead());
    if ((stopped_ = (r < 0))) // Intentional assignment
        return 1;
    if (tape_.writeHead(r >> 8))
        return -1;
    if ((r & 0xFF) == 'L') {
        if (tape_.moveLeft())
            return -1;
    } else if ((r & 0xFF) == 'R') {
        if (tape_.moveRight())
            return -1;
    }
    return 0;
}

int TuringMachine::run()
{
    int r;
    do {} while (!(r = step()));
    return (r < 0) ? r : !register_.onFinal();
}

bool TuringMachine::accepting() const
{
    return register_.onFinal();
}

bool TuringMachine::outOfMemory() const
{
    return tape_.outOfMemory();
}

void TuringMachine::printTape(WINDOW* win, int width)
{
    wmove(win, 0, 0);

    waddch(win, ACS_ULCORNER);
    for (int x = 2; x < width; ++x)
        waddch(win, ACS_HLINE);
    waddch(win, ACS_URCORNER);

    waddch(win, ACS_VLINE);
    tape_.print(win, width - 2);
    waddch(win, ACS_VLINE);

    waddch(win, ACS_LLCORNER);
    for (int x = 2; x < width; ++x)
        waddch(win, ACS_HLINE);
    waddch(win, ACS_LRCORNER);
}

void TuringMachine::printTranscript(WINDOW* win, int width)
{
    std::istringstream ss(register_.transcript());
    std::string line;
    do {
        getline(ss, line);
        if (ss.eof())
            break;

        int color = 0;
        if (line.substr(0, line.find(':')) == register_.getState())
            color = stopped_ ? (accepting() ? 2 : 3) : 1;
        if (color)
            attron(COLOR_PAIR(color));

        int i;
        const char* str = line.c_str();
        for (i = 0; i < width; ++i) {
            if (!*str)
                break;
            waddch(win, *str++);
        }
        for (; i < width; ++i)
            waddch(win, ' ');
        if (color)
            attroff(COLOR_PAIR(color));
    } while (true);
}

void TuringMachine::print(WINDOW* win)
{
    int y, x;
    getmaxyx(win, y, x);
    y = 1;
    printTape(win, x);
    printTranscript(win, x);
    wmove(win, y, x / 2);
}
