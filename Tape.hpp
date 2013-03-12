#ifndef TAPE_HPP
#define TAPE_HPP

#include <cstddef>

constexpr char BLANK = '~';

/** @class Tape
 * This class implements the infinite tape component of the Turing machine
 * (although obviously the storage in this class is finite)
 */
class Tape {
    /** @struct Cell
     * Doubly-linked list representation of an infinite tape. The ends
     * always have sym = BLANK and link to themselves
     */
    struct Cell {
        /** The symbol written on this cell */
        char sym;

        /** Links to the previous and next nodes in the list */
        Cell *prev, *next;

        /** Creates a cell with the given symbol and links. If prev or next
         * are nullptr, the cell is linked to itself to represent the
         * infinite ends of the list
         */
        Cell(char sym, Cell *prev = nullptr, Cell *next = nullptr);
    } *head_;

    /** The current number of cell nodes */
    std::size_t size_;

    /** Duplicates the cell to the left of the head in order to grow the
     * empty space at the left end of the tape
     * @return True if there is an error (i.e., out of memory), false
     * otherwise
     */
    bool duplicateLeft();

    /** Duplicates the cell to the right of the head in order to grow the
     * empty space at the right end of the tape
     * @return True if there is an error (i.e., out of memory), false
     * otherwise
     */
    bool duplicateRight();

    /** Prints n cells to the left of the given cell to win */
    void printLeft(WINDOW* win, Cell *cell, int n);

    /** Prints n cells to the right of the given cell to win */
    void printRight(WINDOW* win, Cell *cell, int n);

public:
    Tape();
    ~Tape();

    /** Moves the head of the tape to the left
     * @return True if there is an error (i.e., out of memory), false
     * otherwise
     */
    bool moveLeft();

    /** Moves the head of the tape to the right
     * @return True if there is an error (i.e., out of memory), false
     * otherwise
     */
    bool moveRight();

    /** Clears the tape to all blanks */
    void clear();

    /** Sets the symbol under the head
     * @return True if there is an error (i.e., out of memory), false
     * otherwise
     */
    bool writeHead(char sym);

    /** Returns the symbol currently under the head */
    char readHead();

    /** Returns whether this tape has allocated all of its available cell
     * nodes. Note that it may still be usable so long no new cells need to
     * be allocated
     */
    bool outOfMemory() const;

    /** Prints the tape to the given curses window at the current position
     * with the head centered at (width / 2)
     */
    void print(WINDOW* win, int width);
};

#endif /* TAPE_HPP */
