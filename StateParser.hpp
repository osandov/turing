#ifndef STATE_PARSER_HPP
#define STATE_PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>

extern std::stringstream err;

/** @stuct ActionDef
 * Used to temporarily store parsed information before resolving state
 * labels
 */
struct ActionDef {
    /** The file in which this action definition was found */
    const char *file;

    /** The line on which this action definition was found */
    int line;
    char sym, replace, shift;
    std::string target;

    ActionDef(const char *file, int line, char sym, char replace,
             char shift, std::string target);
};

class StateRegister;
struct Action;
struct State;

class StateParser {
    /** The register to parse for */
    StateRegister& register_;

    /** A string representation of the register */
    std::string repr_;
    
    /** The name of the file currently being parsed */
    const char* parsingFile_;
    
    /** The state that was last parsed */
    State* parsingState_;

    /** Parses the given istream
     * @return True on failure, false on success
     */
    bool parse(std::istream& stream);

    /** Attempts to parse the given line for a label and adds it to the list
     * of states
     * @return Zero if the line is not a label, negative on an error parsing
     * the label, or positive on success
     */
    int parseLabel(const std::string& line, int n);

    /** Attempts to parse the given line for a rule and adds it to the list
     * of rules for the state currently being parsed
     * @return Zero if the line is not a rule, negative on an error parsing
     * the rule, or positive on success
     */
    int parseRule(const std::string& line, int n);

    /** Resolves all of the action definitions (ActionDef) to actions (Action)
     * by converting each parsed target label to a reference to a State
     * @return True on failure, false on success
     */
    bool resolveSymbols();

    /** Creates the string representation (@see repr_)
     * @note Called automatically by resolveSymbols
     */
    void createRepr();

public:
    StateParser(StateRegister& reg);

    std::string& repr();

    /** Adds all of the rules for the given file and resolves them */
    bool addStates(const char* filename);

    /** Adds all of the rules in each of the num files named in filenames and
     * resolves them
     * @note Inteded for use with argv
     */
    bool addStates(int num, const char* filenames[]);
};

#include "StateRegister.hpp"

#endif /* STATE_PARSER_HPP */
