#include "StateRegister.hpp"

StateRegister::StateRegister() : parser_(*this), currentState_(nullptr) {}

Action::Action(ActionDef& def, State& target) :
    sym(def.sym), replace(def.replace), shift(def.shift), target(target) {}

State::State(std::string label, bool final) :
    label(label), final(final) {}

StateParser& StateRegister::parser()
{
    return parser_;
}

void StateRegister::reset()
{
    currentState_ = &states_.front();
}

int StateRegister::handle(char sym)
{
    for (Action& action : currentState_->table) {
        if (action.sym == sym) {
            currentState_ = &action.target;
            return (action.replace << 8) | action.shift;
        }
    }
    return -1;
}

std::string& StateRegister::transcript()
{
    return parser_.repr();
}

const char *StateRegister::getState() const
{
    return currentState_->label.c_str();
}

bool StateRegister::onFinal() const
{
    return currentState_->final;
}
