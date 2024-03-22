//
//  FA.cpp
//  Regex
//
//  Created by Min on 2024/3/13.
//

#include "FA.hpp"

using namespace FAS;

FA::FA(): FA(0, 0, {}) {}

FA::FA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates): FA(states, {pair<FASymbol, FASymbol>('a', 'z')}, startState, acceptStates) {}

FA::FA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates): states(states), symbols(symbols), startState(startState), acceptStates(acceptStates) {}

bool FA::isSymbolInRange(const FASymbol symbol) const
{
    bool res = false;
    for (const auto& p : symbols) {
        if (symbol >= p.first && symbol <= p.second) {
            res = true;
            break;
        }
    }
    return res;
}

bool FA::isStartState(FAState state) const
{
    return startState == state;
}

bool FA::isAcceptState(FAState state) const
{
    return acceptStates.contains(state);
}

bool FA::containAcceptStates(const unordered_set<FAState>& states) const
{
    bool containAcceptStates = false;
    for (const auto& state: acceptStates) {
        if (states.contains(state)) {
            containAcceptStates = true;
            break;
        }
    }
    return containAcceptStates;
}

void FA::receive(const FASymbol symbol)
{

}

bool FA::recognize(const string& str)
{
    return false;
}

vector<Substring> FA::findRecognizedSubstrings(const string& str)
{
    return {};
}

void FA::simplify(void)
{

}
