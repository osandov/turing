#include <iostream>
#include <sstream>
#include "StateParser.hpp"

constexpr const char* WHITESPACE = " \t";
std::stringstream err;

inline bool isSpace(char c)
{
    return c == ' ' || c == '\t';
}

inline bool isAlNum(char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') || c == '_';
}

ActionDef::ActionDef(const char *file, int line, char sym,
                                 char replace, char shift, std::string target) :
    file(file), line(line), sym(sym), replace(replace), shift(shift),
    target(target) {}

StateParser::StateParser(StateRegister& reg) : register_(reg), 
    parsingFile_(nullptr), parsingState_(nullptr) {}

bool StateParser::addStates(const char *filename)
{
    bool ret = false;
    std::ifstream file(filename);
    repr_.clear();
    parsingFile_ = filename;
    if (!file.is_open())
        err << filename << ": No such file" << std::endl;
    else
        ret |= parse(file);
    return ret | resolveSymbols();
}

bool StateParser::addStates(int num, const char *filenames[])
{
    bool ret = false;
    repr_.clear();
    for (int i = 0; i < num; ++i) {
        std::ifstream file(filenames[i]);
        parsingFile_ = filenames[i];
        if (!file.is_open())
            err << filenames[i] << ": No such file" << std::endl;
        else
            ret |= parse(file);
    }
    return ret | resolveSymbols();
}

bool StateParser::parse(std::istream& stream)
{
    std::string line, temp;
    bool ret = false;
    int n = 0;
    do {
        ++n;
        getline(stream, line);
        if (stream.eof())
            break;
        line.erase(0, line.find_first_not_of(WHITESPACE));
        if (line.empty())
            continue;
        int r = parseLabel(line, n);
        ret |= r < 0;
        if (!r)
            r = parseRule(line, n);
        ret |= r < 0;
        if (!r) {
            err << parsingFile_ << ':' << n
                << ": Invalid syntax" << std::endl;
            ret = true;
        }
    } while (true);
    return ret;
}

int StateParser::parseLabel(const std::string& line, int n)
{
    std::size_t i = line.find(':');
    if (i == std::string::npos)
        return 0;
    bool initial = false, final = false;
    std::string label = line.substr(0, i);
    for (std::size_t j = i + 1; j < line.size(); ++j) {
        char c = line[j];
        if (!isSpace(c)) {
            if (c == 'I') {
                if (register_.currentState_ &&
                    register_.currentState_->label != label)
                {
                    err << parsingFile_ << ':' << n
                        << ": Redefinition of initial state" << std::endl;
                    return -1;
                }
                initial = true;
            } else if (c == 'F')
                final = true;
            else {
                err << parsingFile_ << ':' << n
                    << ": Trailing characters after ':'" << std::endl;
                return -1;
            }
        }
    }
    for (std::size_t j = 0; j < i; ++j) {
        char c = line[j];
        if (isSpace(c)) {
            err << parsingFile_ << ':' << n
                << ": Label contains a space" << std::endl;
            return -1;
        } else if (!isAlNum(c)) {
            err << parsingFile_ << ':' << n
                << ": Label is not alphanumeric" << std::endl;
            return -1;
        }
    }
    for (State& state : register_.states_) {
        if (state.label == label) {
            err << parsingFile_ << ':' << n
                << ": State with label `" << label
                << "' has already been defined" << std::endl;
            return -1;
        }
    }
    if (initial) {
        register_.states_.emplace_front(label, final);
        parsingState_ = register_.currentState_ = &register_.states_.front();
    } else {
        register_.states_.emplace_back(label, final);
        parsingState_ = &register_.states_.back();
    }
    return 1;
}

int StateParser::parseRule(const std::string& line, int n)
{
    if (!parsingState_) {
        err << parsingFile_ << ':' << n
            << ": Orphaned rule" << std::endl;
        return -1;
    }

    char sym = line[0], replace, shift;
    std::size_t i, j;

    // Match the replacement character
    for (i = 1; i < line.size(); ++i) {
        if (!isSpace(line[i])) {
            replace = line[i];
            break;
        }
    }
    if (i >= line.size()) {
        err << parsingFile_ << ':' << n
            << ": Missing replacement character" << std::endl;
        return -1;
    }

    // Match the shift
    for (++i; i < line.size(); ++i) {
        if (!isSpace(line[i])) {
            shift = line[i];
            break;
        }
    }
    if (i >= line.size()) {
        err << parsingFile_ << ':' << n
            << ": Missing shift" << std::endl;
        return -1;
    } else if (shift != 'L' && shift != 'R') {
        err << parsingFile_ << ':' << n
            << ": Direction must be 'L' or 'R'" << std::endl;
        return -1;
    }

    // Match the '-' in '->'
    for (++i; i < line.size(); ++i) {
        char c = line[i];
        if (!isSpace(c)) {
            if (c == '-') {
                ++i;
                break;
            } else {
                err << parsingFile_ << ':' << n
                    << ": Extraneous characters before '->'"
                    << std::endl;
                return -1;
            }
        }
    }
    // Check for the '>' in '->'
    if (i >= line.size() || line[i] != '>') {
        err << parsingFile_ << ':' << n << ": Missing '->'"
            << std::endl;
        return -1;
    }
    // Find the beginning of the target label
    for (++i; i < line.size(); ++i) {
        if (!isSpace(line[i]))
            break;
    }
    if (i >= line.size()) {
        err << parsingFile_ << ':' << n
            << ": Missing target state" << std::endl;
        return -1;
    }
    // Find the end of the target label
    for (j = i; j < line.size(); ++j) {
        if (isSpace(line[j]))
            break;
    }
    // Check for trailing characters
    for (std::size_t k = j + 1; k < line.size(); ++k) {
        if (!isSpace(line[k])) {
            err << parsingFile_ << ':' << n
                << ": Trailing characters after target state"
                      << std::endl;
            return -1;
        }
    }
    parsingState_->actionDefs.emplace_front(
        parsingFile_, n, sym, replace, shift, line.substr(i, j - i));
    return 1;
}

bool StateParser::resolveSymbols()
{
    bool ret = !register_.currentState_;
    for (State& state : register_.states_) {
        for (ActionDef& def : state.actionDefs) {
            bool found = false;
            for (State& search : register_.states_) {
                if (def.target == search.label) {
                    found = true;
                    state.table.emplace_front(def, search);
                    break;
                }
            }
            if (!found) {
                err << def.file << ':' << def.line
                          << ": Unrecognized label `" << def.target
                          << "'" << std::endl;
                ret = true;
            }
        }
        state.actionDefs.clear();
    }
    if (!register_.currentState_) {
        err << ":: No initial state defined" << std::endl;
        repr_.clear();
    } else
        createRepr();
    return ret;
}

void StateParser::createRepr()
{
    std::stringstream ss;
    bool initial = true;
    for (const State& state : register_.states_) {
        if (!initial)
            ss << '\n';
        ss << state.label << ':';
        if (initial) {
            ss << 'I';
            initial = false;
        }
        if (state.final)
            ss << 'F';
        ss << '\n';
        for (const Action& action : state.table) {
            ss << "    " << action.sym << ' ' << action.replace << ' '
               << action.shift << " -> " << action.target.label << '\n';
        }
    }
    repr_ = ss.str();
}

std::string& StateParser::repr()
{
    return repr_;
}
