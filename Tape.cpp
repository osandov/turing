#include <curses.h>
#include "Tape.hpp"

/** Although the tape should theoretically be infinite, we have to limit its
 * size in order to prevent an infinite loop from consuming all of the
 * system's memory
 */
#ifdef MAX_TAPE_CELLS
constexpr std::size_t MAX_CELLS = MAX_TAPE_CELLS;
#else
constexpr std::size_t MAX_CELLS = 1000000;
#endif /* MAX_TAPE_CELLS */

Tape::Cell::Cell(char sym, Cell *prev, Cell *next) :
    sym(sym), prev(prev ? prev : this), next(next ? next : this) {}

Tape::Tape() : head_(new Cell(BLANK)), size_(1) {}

Tape::~Tape()
{
    Cell *back = head_, *front = head_;
    while (back->prev != back)
        back = back->prev;
    while (front->next != front)
        front = front->next;
    if (back == front)
        delete front;
    else {
        Cell *cell = back->next;
        while (cell != front) {
            Cell *next = cell->next;
            delete cell;
            cell = next;
        }
        delete back;
        delete front;
    }
}

bool Tape::duplicateLeft()
{
    if (size_ >= MAX_CELLS)
        return true;
    ++size_;
    head_->prev = new Cell(head_->sym, nullptr, head_);
    return false;
}

bool Tape::duplicateRight()
{
    if (size_ >= MAX_CELLS)
        return true;
    ++size_;
    head_->next = new Cell(head_->sym, head_, nullptr);
    return false;
}

bool Tape::moveLeft()
{
    if ((head_->prev == head_) && duplicateLeft())
        return true;
    head_ = head_->prev;
    return false;
}

bool Tape::moveRight()
{
    if ((head_->next == head_) && duplicateRight())
        return true;
    head_ = head_->next;
    return false;
}

void Tape::clear()
{
    Cell* cell = head_;
    while (cell->prev != cell) {
        cell->sym = BLANK;
        cell = cell->prev;
    }
    while (head_->next != head_) {
        head_->sym = BLANK;
        head_ = head_->next;
    }
    for (std::size_t i = 0; i < size_ / 2; ++i) // Center the head
        head_ = head_->prev;
}

bool Tape::writeHead(char sym)
{
    if (((head_->prev == head_) && duplicateLeft()) ||
        ((head_->next == head_) && duplicateRight()))
        return true;
    head_->sym = sym;
    return false;
}

char Tape::readHead()
{
    return head_->sym;
}

bool Tape::outOfMemory() const
{
    return size_ >= MAX_CELLS;
}

void Tape::printLeft(WINDOW* win, Cell *cell, int n)
{
    if (n) {
        printLeft(win, cell->prev, n - 1);
        waddch(win, cell->prev->sym);
    }
}

void Tape::printRight(WINDOW* win, Cell *cell, int n)
{
    for (int i = 0; i < n; ++i, cell = cell->next)
        waddch(win, cell->next->sym);
}

void Tape::print(WINDOW* win, int width)
{
    printLeft(win, head_, width / 2);
    waddch(win, head_->sym);
    printRight(win, head_, (width - 1) / 2);
}
