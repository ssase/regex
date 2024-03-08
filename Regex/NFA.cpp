//
//  NFA.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#include "NFA.hpp"

const unordered_set<NFAState>& NFA::receive(const NFAState currentState, const NFASymbol symbol)
{
    return delta[currentState][symbol];
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
            const unordered_set<NFAState>& tempResult = delta[*header][EPSILON];
            temp.insert(tempResult.begin(), tempResult.end());
            header++;
        }
        states = temp;
        temp.clear();
        result.insert(states.begin(), states.end());
    }

    return result;
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
        result.push_back({0, 0});
    }

    parsingStartStates = startStates;

    while (parser < str.end()) {

        for (const auto& state : parsingStartStates) {
            const unordered_set<NFAState> &deltaResult = receive(state, *parser);
            tempStates.insert(deltaResult.begin(), deltaResult.end());
        }
        emptyReachableStates = collectEmptyCharReachableStates(tempStates);
        tempStates.insert(emptyReachableStates.begin(), emptyReachableStates.end());
        splitedNFAStates = tempStates;

        // Clear after used.
        tempStates.clear();

        if (splitedNFAStates.empty()) {

            if (parsingStartStates == startStates) {
                printf("\n======== parser++\n");
                parser++;

            } else {
                if (matchedSubstring.second != NothingMatched) {
                    result.push_back({(NFAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
                }
                parsingStartStates = startStates;
            }

            matchedSubstring = {parser, NothingMatched};

        } else {

            printf("\n======== parser++\n");
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
