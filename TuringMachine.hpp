#ifndef TURING_MACHINE_HPP
#define TURING_MACHINE_HPP

#include <curses.h>
#include "StateRegister.hpp"
#include "Tape.hpp"

class TuringMachine {
    /** The register of states */
    StateRegister register_;

    /** The infinite tape */
    Tape tape_;

    /** Whether the machine stopped execution (i.e., attempted to step and no
     * action was found
     */
    bool stopped_;

    void printTape(WINDOW* win, int width);
    void printTranscript(WINDOW* win, int width);

public:
    TuringMachine();

    StateParser& parser();

    /** Clears the tape and writes the given string to it, positioning the
     * head at the front of the string
     * @param str Null-terminated string containing only human-readable
     * characters
     */
    void write(const char* str);

    /** Executes one action based on the current state and the symbol under
     * the head
     * @return Zero if an action executed successfully, positive if no
     * applicable action was found, and negative if there was an error
     */
    int step();

    /** Executes actions until another action can no longer be handled
     * @return Zero if execution completed on a final state, positive if the
     * machine jammed, and negative if there was an error
     */
    int run();

    /** Whether this machine is in an accepting (a.k.a. final) state */
    bool accepting() const;

    /** Whether the tape is out of memory; @see Tape::outOfMemory() */
    bool outOfMemory() const;

    /** Prints the tape and a view of the state register to the given curses
     * window
     */
    void print(WINDOW* win);
};

#endif /* TURING_MACHINE_HPP */
