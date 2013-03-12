#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <string>
#include <list>
#include "StateParser.hpp"

struct State;

/** @struct Action
 * An action that changes the state of a finite state machine
 */
struct Action {
    /** The symbol which must be read for the action to execute */
    char sym;

    /** The symbol to replace the read symbol with */
    char replace;

    /** The direction to move the tape after changing the read symbol
     * (either 'L' or 'R')
     */
    char shift;

    /** The state to move to when executing the action */
    State& target;

    /** Constructs an action from a definition and a target state */
    Action(ActionDef& def, State& target);
};

/** @struct State
 * A state of a finite state machine
 */
struct State {
    /** This state's name */
    std::string label;

    /** Whether this state is a final (accepting) state */
    bool final;

    /** A list of action definitions which is added to when parsing and
     * freed after resolving
     */
    std::list<ActionDef> actionDefs;
    
    /** The table of actions for this state */
    std::list<Action> table;

    /** Constructs a state with an empty table
     * @param label The name for the state
     * @param final Whether this state should be an accepting state
     */
    State(std::string label, bool final);
};

/** @class StateRegister
 * This class implements the finite state machine portion of the turing
 * machine. It also provides the parser for the scripting language
 */
class StateRegister {
    /** A parser for this register */
    StateParser parser_;

    /** All of the states for this finite state machine. The initial state is
     * always at the front of this list
     */
    std::list<State> states_;
    
    /** The state this machine is currently in */
    State* currentState_;

public:
    StateRegister();

    StateParser& parser();

    /** Restores the current state to the initial state */
    void reset();

    /** Handles the given event symbol by advancing the state if there is a
     * match and returning the data that needs to be changed
     * @return If there is a match, an integer with the shift in the least
     * significant byte and the replacement character in the adjacent byte.
     * If there is no match, a negative integer
     * @note Behavior is undefined if an initial state has not been defined
     */
    int handle(char sym);

    /** Returns a string representation of this machine's states and rules in
     * the canonical format
     */
    std::string& transcript();

    /** Returns the label of the current state
     * @note Behavior is undefined if an initial state has not been defined
     */
    const char* getState() const;

    /** Returns whether the current state is final (accepting) */
    bool onFinal() const;

    friend class StateParser;
};

#endif /* STATE_MACHINE_HPP */
