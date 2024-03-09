//
//  NFA.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#include "NFA.hpp"

const unordered_set<NFAState> NFA::receive(const NFAState currentState, const NFASymbol symbol)
{
    if (delta.size() > currentState && delta[currentState].contains(symbol)) {
        return delta[currentState][symbol];
    }
    return {};
}

bool NFA::isStartState(NFAState state)
{
    return startState == state;
}

bool NFA::isAcceptState(NFAState state)
{
    return acceptStates.contains(state);
}

bool NFA::containAcceptStates(const unordered_set<NFAState>& states)
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

unordered_set<NFAState> NFA::collectEmptyCharReachableStates(unordered_set<NFAState> states)
{
    unordered_set<NFAState> result;
    unordered_set<NFAState> temp;

    while (!states.empty()) {
        auto header = states.begin();
        while (header != states.end()) {
            const unordered_set<NFAState> tempResult = receive(*header, EPSILON);
            temp.insert(tempResult.begin(), tempResult.end());
            header++;
        }
        states = temp;
        temp.clear();
        result.insert(states.begin(), states.end());
    }

    return result;
}

NFA::NFA(const NFASymbol specialSymbol)
{
    if ((specialSymbol >= 'a' && specialSymbol <= 'z') || (specialSymbol >= 'A' && specialSymbol <= 'Z')) {
        Q = 2;
        startState = 0;
        acceptStates = {1};
        delta = {
            {{specialSymbol, {1}}},
            {}
        };
    } else if (specialSymbol == '.') {
        Q = 2;
        startState = 0;
        acceptStates = {1};
        delta = {
            {},
            {}
        };
        for (char c = 'a'; c <= 'z'; c++) {
            delta[0][c] = {1};
        }
    } else {
        Q = 0;
        startState = 0;
        acceptStates = {};
        delta = {};
    }
}

NFA::NFA(const string pattern)
{
    *this = NFA{};
    NFA n1;
    NFA m{'.'};

    char t = '*';

    for (auto i = pattern.begin(); i <= pattern.end(); i++) {

        if (t != '*') {

            if (t == '.') {
                n1 = m;

            } else {

                n1 = NFA(2, 0, {1}, {
                    {{t, {1}}},
                    {},
                });
            }

            if (*i == '*') {
                n1.makeStar();
            }

            this->makeConcatenation(n1);
        }

        t = *i;
    }
}

const NFASymbol NFA::EPSILON = -1; // We use it as an ε which means empty symbol in an NFA.

vector<Substring> NFA::parseString(const string& str)
{
    const unsigned int NothingMatched = -1;

    // `pair.first` means the first location of matched substring in the string, and `pair.second` means the length.
    // `result` stores all matched substring,
    vector<Substring> result;
    // and `matchedSubstring` stores substring that is being analysed.
    pair<string::const_iterator, unsigned int> matchedSubstring{str.begin(), NothingMatched};

    unordered_set<NFAState> deltaResult;

    // Although there is only one start state, but we should also consider ε(aka empty string), this will store the start states after considering empty string reachability.
    unordered_set<NFAState> startStates{startState};

    // Before the parsing, the NFA's states
    unordered_set<NFAState> parsingStartStates;
    // During parsing, we need to store the result states as splited NFAs.
    unordered_set<NFAState> splitedNFAStates;

    // And also, we need to store result states before finishing calculating states in splitedNFAStates.
    unordered_set<NFAState> tempStates;
    unordered_set<NFAState> emptyReachableStates;

    // Follow the symbol under analysed.
    string::const_iterator parser = str.begin();

    // Calculate startStates
    emptyReachableStates = collectEmptyCharReachableStates(startStates);
    startStates.insert(emptyReachableStates.begin(), emptyReachableStates.end());
    // To find out if the NFA accepts empty string.
    if (containAcceptStates(startStates)) {
        matchedSubstring.second = 0;
    }

    parsingStartStates = startStates;

    while (parser < str.end()) {

        for (const auto& state : parsingStartStates) {
            deltaResult = receive(state, *parser);
            tempStates.insert(deltaResult.begin(), deltaResult.end());
        }
        emptyReachableStates = collectEmptyCharReachableStates(tempStates);
        tempStates.insert(emptyReachableStates.begin(), emptyReachableStates.end());
        splitedNFAStates = tempStates;

        // Clear after used.
        tempStates.clear();

        if (splitedNFAStates.empty()) {

            if (parsingStartStates == startStates) {
                parser++;

            } else {
                if (matchedSubstring.second != NothingMatched) {
                    result.push_back({(NFAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
                }
                parsingStartStates = startStates;
            }

            matchedSubstring = {parser, NothingMatched};

        } else {

            parser++;
            parsingStartStates = splitedNFAStates;

            if (containAcceptStates(splitedNFAStates)) {
                matchedSubstring.second = (NFAState)(parser - matchedSubstring.first);
            }
        }
    }

    if (matchedSubstring.second != NothingMatched) {
        result.push_back({(NFAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
    }

    return result;
}

void NFA::makeUnion(const NFA& n)
{
    if (n.Q == 0) { return;}
    if (Q == 0) {
        *this = n;
        return;
    }

    startState = 0;

    unordered_set<NFAState> newAcceptStates;
    for (const auto& state: acceptStates) {
        newAcceptStates.insert(state + 1);
    }
    for (const auto& state: n.acceptStates) {
        newAcceptStates.insert(state + 1 + Q);
    }
    acceptStates = newAcceptStates;

    vector<unordered_map<NFASymbol, unordered_set<NFAState>>> newDelta;
    unordered_map<NFASymbol, unordered_set<NFAState>> tempMap;
    unordered_set<NFAState> tempSet;
    newDelta.push_back({{EPSILON, {startState + 1, n.startState + 1 + Q}}});

    const vector<unordered_map<NFASymbol, unordered_set<NFAState>>> * deltas[] = {&delta, &n.delta};
    const NFAState addons[] = {1, Q + 1};
    for (int i = 0; i < 2; i++) {
        for (const auto& map : *(deltas[i])) {
            for (const auto& pair : map) {
                for (const auto& state : pair.second) {
                    tempSet.insert(state + addons[i]);
                }
                tempMap[pair.first] = tempSet;
                tempSet.clear();
            }
            newDelta.push_back(tempMap);
            tempMap.clear();
        }
    }
    delta = newDelta;

    Q += n.Q + 1;
}

void NFA::makeConcatenation(const NFA& n)
{
    if (n.Q == 0) { return;}
    if (Q == 0) {
        *this = n;
        return;
    }

    for (const auto& state: acceptStates) {
        delta[state][EPSILON].insert(n.startState + Q);
    }
    unordered_map<NFASymbol, unordered_set<NFAState>> tempMap;
    unordered_set<NFAState> tempSet;
    for (const auto& map : n.delta) {
        for (const auto& pair : map) {
            for (const auto& state : pair.second) {
                tempSet.insert(state + Q);
            }
            tempMap[pair.first] = tempSet;
            tempSet.clear();
        }
        delta.push_back(tempMap);
        tempMap.clear();
    }

    unordered_set<NFAState> newAcceptStates;
    for (const auto& state: n.acceptStates) {
        newAcceptStates.insert(state + Q);
    }
    acceptStates = newAcceptStates;

    Q += n.Q;
}

void NFA::makeStar(void)
{
    if (Q == 0) { return; }

    for (const auto& state: acceptStates) {
        delta[state][EPSILON].insert(startState);
    }
    acceptStates = {startState};
}
