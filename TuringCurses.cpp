#include "TuringCurses.hpp"

TuringCurses::TuringCurses() : stdscr_(nullptr) {}

TuringCurses::~TuringCurses()
{
    if (stdscr_) {
        delwin(status_);
        endwin();
    }
}

void TuringCurses::initCurses()
{
    stdscr_ = initscr();
    keypad(stdscr_, true);
    cbreak();
    noecho();

    updateSize();

    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_WHITE, COLOR_RED);
}

bool TuringCurses::addStates(const char* filename)
{
    return machine_.parser().addStates(filename);
}

void TuringCurses::drawScreen()
{
    machine_.print(stdscr_);
    wrefresh(stdscr_);
    wrefresh(status_);
}

void TuringCurses::readInput()
{
    static const std::string prompt("Input? ");
    std::string input;
    int c;
    writeStatus(prompt.c_str());
    while ((c = wgetch(status_)) != '\n') {
        int y, x;
        getyx(status_, y, x);
        if (c >= 0x20 && c < 0x7F) {
            mvwinsch(status_, y, x, c);
            wmove(status_, y, x + 1);
            input.insert(x - prompt.size(), 1, (char)c);
        } else if (c == KEY_BACKSPACE && (unsigned)x > prompt.size()) {
            input.erase(x - prompt.size() - 1, 1);
            mvwdelch(status_, y, x - 1);
        } else if (c == KEY_LEFT && (unsigned)x > prompt.size())
            wmove(status_, y, x - 1);
        else if (c == KEY_RIGHT && (unsigned)x < input.size() + prompt.size())
            wmove(status_, y, x + 1);
    }
    machine_.write(input.c_str());
    writeStatus("Machine is idle");
}

void TuringCurses::updateSize()
{
    werase(stdscr_);
    delwin(status_);
    getmaxyx(stdscr_, height_, width_);
    status_ = newwin(1, width_, height_ - 1, 0);
    keypad(status_, true);
}

void TuringCurses::writeStatus(const char* message)
{
    werase(status_);
    wmove(status_, 0, 0);
    waddstr(status_, message);
}

int TuringCurses::main()
{
    drawScreen();
    readInput();

    int c;
    do {
        drawScreen();
        wmove(stdscr_, 1, width_ / 2);

        if ((c = getch()) == 'q')
            break;
        else if (c == KEY_RESIZE)
            updateSize();

        int result = 0;
        if (c == 'n')
            result = machine_.step();
        else if (c == '\n') {
            result = 1;
            if (machine_.run() < 0)
                result = -1;
        } else if (c == 'i') {
            readInput();
            continue;
        } else
            continue;
        if (result < 0) {
            if (machine_.outOfMemory())
                writeStatus("Out-of-memory error");
            else
                writeStatus("Unspecified error occurred");
            waddstr(status_, "; press any key to reset");
            drawScreen();
            wgetch(status_);
            readInput();
        } else if (result > 0) {
            if (!machine_.accepting())
                writeStatus("Machine jammed");
            else
                writeStatus("Execution completed; machine is accepting");
        } else
            writeStatus("Machine is running");
    } while (true);
    return 0;
}
