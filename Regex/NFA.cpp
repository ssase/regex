//
//  NFA.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#include "NFA.hpp"
#include "DFA.hpp"

using namespace FAS;

const unordered_set<FAState> NFA::transitResult(FAState state, FASymbol symbol) const
{
    if (state < 0 || state >= states) { return {}; }
    const auto& map = transition[state];
    if (!map.contains(symbol)) { return {}; }
    return map.at(symbol);
}

const unordered_set<FAState> NFA::transitResult(unordered_set<FAState> states, FASymbol symbol) const
{
    unordered_set<FAState> temp;
    for (auto tr = states.begin(); tr != states.end(); tr++) {
        const unordered_set<FAState>&& tempResult = transitResult(*tr, symbol);
        temp.insert(tempResult.begin(), tempResult.end());
    }
    return temp;
}

unordered_set<FAState> NFA::collectEmptySymbolReachableStates(unordered_set<FAState> states) const
{
    unordered_set<FAState> result = states;
    unordered_set<FAState> temp;

    while (!states.empty()) {
        auto header = states.begin();
        while (header != states.end()) {
            const unordered_set<FAState>& tempResult = transitResult(*header, EPSILON);
            for (auto it = tempResult.begin(); it != tempResult.end(); it++) {
                if (!result.contains(*it)) {
                    temp.insert(*it);
                }
            }
            header++;
        }
        states = temp;
        temp.clear();
        result.insert(states.begin(), states.end());
    }

    return result;
}

const FASymbol NFA::EPSILON = -1; // We use it as an ε which means empty symbol in an NFA.

NFA::NFA(): NFA(0, 0, {}, {{}}) {}

NFA::NFA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, unordered_set<FAState>>>& transition): FA(states, startState, acceptStates), transition(transition) {
    calculateCurrentStates();
}

NFA::NFA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, unordered_set<FAState>>>& transition): FA(states, symbols, startState, acceptStates), transition(transition) {
    calculateCurrentStates();
}

NFA::NFA(const FASymbol specialSymbol): NFA()
{
    if ((specialSymbol >= 'a' && specialSymbol <= 'z') || (specialSymbol >= 'A' && specialSymbol <= 'Z')) {
        states = 2;
        startState = 0;
        acceptStates = {1};
        transition = {
            {{specialSymbol, {1}}},
            {}
        };
    } else if (specialSymbol == '.') {
        states = 2;
        startState = 0;
        acceptStates = {1};
        transition = {
            {},
            {}
        };
        for (char c = 'a'; c <= 'z'; c++) {
            transition[0][c] = {1};
        }
    }

    completeChanging();
}

// Init an NFA with a regex
NFA::NFA(const string pattern): NFA()
{
    NFA n;

    char t = '*';

    for (auto i = pattern.begin(); i <= pattern.end(); i++) {

        if (t != '*') {

            if (t == '.') {
                n = NFA{'.'};

            } else {

                n = NFA(t);
            }

            if (*i == '*') {
                n.makeStar();
            }

            makeConcatenation(n);
        }

        t = *i;
    }

    completeChanging();
}

NFA::NFA(const DFA& d)
{
    states = d.states;
    symbols = d.symbols;
    startState = d.startState;
    acceptStates = d.acceptStates;
    transition = vector<unordered_map<FASymbol, unordered_set<FAState>>>(d.transition.size(), unordered_map<FASymbol, unordered_set<FAState>>());
    for (int i = 0; i < transition.size(); i++) {
        for (auto it = d.transition[i].begin(); it != d.transition[i].end(); it++) {
            transition[i][it->first] = {it->second};
        }
    }

    calculateCurrentStates();
}

void NFA::receive(const FASymbol symbol)
{
    currentStates = collectEmptySymbolReachableStates(transitResult(currentStates, symbol));
}

vector<Substring> NFA::recognize(const string& str)
{
    const unsigned int NothingMatched = -1;

    // `pair.first` means the first location of matched substring in the string, and `pair.second` means the length.
    // `result` stores all matched substring,
    vector<Substring> result;
    // and `matchedSubstring` stores substring that is being analysed.
    pair<string::const_iterator, unsigned int> matchedSubstring{str.begin(), NothingMatched};

    unordered_set<FAState> startStates = currentStates;
    bool beginFromStartStates = false;

    // Follow the symbol under analysed.
    string::const_iterator parser = str.begin();

    // To find out if the NFA accepts empty string.
    if (containAcceptStates(currentStates)) {
        matchedSubstring.second = 0;
    }

    while (parser < str.end()) {

        receive(*parser);

        if (currentStates.empty()) {

            if (matchedSubstring.second != NothingMatched) {
                result.push_back({(FAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
            }
            if (beginFromStartStates) {
                parser++;
            }
            beginFromStartStates = true;
            currentStates = startStates;
            matchedSubstring = {parser, NothingMatched};

        } else {

            beginFromStartStates = false;
            parser++;
            if (containAcceptStates(currentStates)) {
                matchedSubstring.second = (unsigned int)(parser - matchedSubstring.first);
            }
        }
    }

    if (matchedSubstring.second != NothingMatched) {
        result.push_back({(FAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
    }

    return result;
}

void NFA::makeUnion(const NFA& n)
{
    if (n.states == 0) { return;}
    if (states == 0) {
        *this = n;
        return;
    }

    startState = 0;

    unordered_set<FAState> newAcceptStates;
    for (const auto& state: acceptStates) {
        newAcceptStates.insert(state + 1);
    }
    for (const auto& state: n.acceptStates) {
        newAcceptStates.insert(state + 1 + states);
    }
    acceptStates = newAcceptStates;

    vector<unordered_map<FASymbol, unordered_set<FAState>>> newTransition;
    unordered_map<FASymbol, unordered_set<FAState>> tempMap;
    unordered_set<FAState> tempSet;
    newTransition.push_back({{EPSILON, {startState + 1, n.startState + 1 + states}}});

    const vector<unordered_map<FASymbol, unordered_set<FAState>>> * transitions[] = {&transition, &n.transition};
    const FAState addons[] = {1, states + 1};
    for (int i = 0; i < 2; i++) {
        for (const auto& map : *(transitions[i])) {
            for (const auto& pair : map) {
                for (const auto& state : pair.second) {
                    tempSet.insert(state + addons[i]);
                }
                tempMap[pair.first] = tempSet;
                tempSet.clear();
            }
            newTransition.push_back(tempMap);
            tempMap.clear();
        }
    }
    transition = newTransition;

    states += n.states + 1;

    completeChanging();
}

void NFA::makeConcatenation(const NFA& n)
{
    if (n.states == 0) { return;}
    if (states == 0) {
        *this = n;
        return;
    }

    for (const auto& state: acceptStates) {
        transition[state][EPSILON].insert(n.startState + states);
    }
    unordered_map<FASymbol, unordered_set<FAState>> tempMap;
    unordered_set<FAState> tempSet;
    for (const auto& map : n.transition) {
        for (const auto& pair : map) {
            for (const auto& state : pair.second) {
                tempSet.insert(state + states);
            }
            tempMap[pair.first] = tempSet;
            tempSet.clear();
        }
        transition.push_back(tempMap);
        tempMap.clear();
    }

    unordered_set<FAState> newAcceptStates;
    for (const auto& state: n.acceptStates) {
        newAcceptStates.insert(state + states);
    }
    acceptStates = newAcceptStates;

    states += n.states;

    completeChanging();
}

void NFA::makeStar(void)
{
    if (states == 0) { return; }

    for (const auto& state: acceptStates) {
        transition[state][EPSILON].insert(startState);
    }
    acceptStates = {startState};

    calculateCurrentStates();
}

void NFA::calculateCurrentStates(void)
{
    currentStates = collectEmptySymbolReachableStates({startState});
}

void NFA::simplify(void)
{
    DFA d = DFA(*this);
    *this = NFA(d);
}

void NFA::completeChanging(void)
{
    simplify();
    calculateCurrentStates();
}
