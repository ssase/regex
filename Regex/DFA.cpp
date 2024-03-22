//
//  DFA.cpp
//  Regex
//
//  Created by Min on 2024/3/12.
//

#include <queue>

#include "DFA.hpp"
#include "NFA.hpp"
#include "BitNumber.hpp"

using std::queue;

using namespace FAS;

struct StateGroupInfo {
    FAState groupNum;
    FAState resultGroupNum;

    bool operator==(const StateGroupInfo& info) const {
        return groupNum == info.groupNum && resultGroupNum == info.resultGroupNum;
    }
};

namespace std {
    template <>
    struct hash<StateGroupInfo> {
        std::size_t operator()(const StateGroupInfo& info) const {
            return hash_val(info.groupNum, info.resultGroupNum);
        }
    };
}

FAState DFA::transitResult(FAState state, FASymbol symbol)
{
    if (state < 0 || state >= states) { return StateNotFound; }
    auto& map = transition[state];
    if (!map.contains(symbol)) { return StateNotFound; }
    return map[symbol];
}

bool DFA::isTerminalState(FAState state)
{
    return terminalStates.contains(state);
}

void DFA::calculateTerminalStates(void)
{
    terminalStates = {};
    bool need;
    for (FAState s = 0; s < states; s++) {
        if (!isAcceptState(s)) {
            const auto& map = transition[s];
            need = true;
            for (auto symbolRange: symbols) {
                for (auto i = symbolRange.first; i <= symbolRange.second; i++) {
                    if (map.at(i) != s) {
                        need = false;
                        break;
                    }
                }
                if (!need) {
                    break;
                }
            }
            if (need) {
                terminalStates.insert(s);
            }
        }
    }
}

DFA::DFA(): DFA(0, 0, {}, {}) {}

DFA::DFA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition): FA(states, startState, acceptStates), currentState(startState), transition(transition)
{
    calculateTerminalStates();
}

DFA::DFA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition): FA(states, symbols, startState, acceptStates), currentState(startState), transition(transition)
{
    calculateTerminalStates();
}

DFA::DFA(const NFA& n)
{
    symbols = n.symbols;
    startState = 0;
    currentState = 0;
    acceptStates = {};
    transition = {};

    unordered_map<FAState, unordered_map<FASymbol, BitNumber>> tempTransition;
    queue<unordered_set<FAState>> statesQueue;
    statesQueue.push(n.collectEmptySymbolReachableStates({startState}));

    FAState index = 0;
    unordered_map<BitNumber, FAState> stateMap;

    unordered_set<FAState> stateSet;
    unordered_set<FAState> tempStateSet;
    BitNumber stateNumber;

    while (!statesQueue.empty()) {

        stateSet = statesQueue.front();
        statesQueue.pop();
        stateNumber = BitNumber(stateSet);

        if (!stateMap.contains(stateNumber)) {

            stateMap[stateNumber] = index;
            if (n.containAcceptStates(stateSet)) {
                acceptStates.insert(index);
            }

            for (auto symbolRange: symbols) {
                for (auto i = symbolRange.first; i <= symbolRange.second; i++) {
                    tempStateSet = n.collectEmptySymbolReachableStates(n.transitResult(stateSet, i));
                    statesQueue.push(tempStateSet);
                    tempTransition[index][i] = BitNumber(tempStateSet);
                }
            }

            index++;
        }
    }

    unordered_map<FASymbol, FAState> tempMap;
    for (FAState i = 0; i < index; i++) {
        for (auto symbolRange: symbols) {
            for (auto j = symbolRange.first; j <= symbolRange.second; j++) {
                tempMap[j] = stateMap[tempTransition[i][j]];
            }
        }
        transition.push_back(tempMap);
        tempMap.clear();
    }

    states = (FAState)transition.size();

    simplify();
}

void DFA::receive(const FASymbol symbol)
{
    currentState = transitResult(currentState, symbol);
}

bool DFA::recognize(const string& str)
{
    resetCurrentState();
    for (auto it = str.begin(); it != str.end(); it++) {
        receive(*it);
    }
    return isAcceptState(currentState);
}

vector<Substring> DFA::findRecognizedSubstrings(const string& str)
{
    resetCurrentState();

    const unsigned int NothingMatched = -1;

    // `pair.first` means the first location of matched substring in the string, and `pair.second` means the length.
    // `result` stores all matched substring,
    vector<Substring> result;
    // and `matchedSubstring` stores substring that is being analysed.
    pair<string::const_iterator, unsigned int> matchedSubstring{str.begin(), NothingMatched};

    FAState beginState = currentState;
    bool beginFromStartStates = false;

    // Follow the symbol under analysed.
    string::const_iterator parser = str.begin();

    // To find out if the DFA accepts empty string.
    if (isAcceptState(currentState)) {
        matchedSubstring.second = 0;
    }

    while (parser < str.end()) {

        receive(*parser);

        if (isTerminalState(currentState)) {

            if (matchedSubstring.second != NothingMatched) {
                result.push_back({(FAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
            }
            if (currentState == beginState) {
                parser++;
            }
            beginFromStartStates = true;
            currentState = beginState;
            matchedSubstring = {parser, NothingMatched};

        } else {

            beginFromStartStates = false;
            parser++;
            if (isAcceptState(currentState)) {
                matchedSubstring.second = (unsigned int)(parser - matchedSubstring.first);
            }
        }
    }

    if (matchedSubstring.second != NothingMatched) {
        result.push_back({(FAState)(matchedSubstring.first - str.begin()), matchedSubstring.second});
    }

    return result;
}

void DFA::makeUnion(const DFA& d)
{
    NFA n1 = NFA(*this);
    NFA n2 = NFA(d);
    n1.makeUnion(n2);
    DFA d2 = DFA(n1);
    d2.simplify();
    *this = d2;
}

void DFA::makeConcatenation(const DFA& d)
{
    NFA n1 = NFA(*this);
    NFA n2 = NFA(d);
    n1.makeConcatenation(n2);
    DFA d2 = DFA(n1);
    d2.simplify();
    *this = d2;
}

void DFA::makeStar(void)
{
    NFA n1 = NFA(*this);
    n1.makeStar();
    DFA d2 = DFA(n1);
    d2.simplify();
    *this = d2;
}

void DFA::simplify(void)
{
    vector<StateGroupInfo> groups = vector<StateGroupInfo>(states, StateGroupInfo{0, 0});
    FAState acceptStatesGroupNum = acceptStates.size() == states ? 0 : 1;
    for (FAState s = 0; s < states; s++) {
        if (isAcceptState(s)) {
            groups[s].groupNum = acceptStatesGroupNum;
        }
    }

    unordered_map<StateGroupInfo, FAState> newStateGroupMap;
    FAState resState;
    FAState currentStateCount = acceptStatesGroupNum + 1;

    for (int i = 0; i < symbols.size(); i++) {

        FASymbol symbol = symbols[i].first;
        while (symbol <= symbols[i].second) {

            newStateGroupMap.clear();

            for (int j = 0; j < states; j++) {

                resState = transition[j][symbol];
                groups[j].resultGroupNum = groups[resState].groupNum;
                if (newStateGroupMap.find(groups[j]) == newStateGroupMap.end()) {
                    newStateGroupMap[groups[j]] = (FAState)newStateGroupMap.size();
                }
            }

            if (newStateGroupMap.size() > currentStateCount) {

                for (int j = 0; j < states; j++) {
                    groups[j].groupNum = newStateGroupMap[groups[j]];
                }
                currentStateCount = (FAState)newStateGroupMap.size();

            } else {
                symbol++;
            }
        }
    }

    unordered_map<FAState, unordered_map<FASymbol, FAState>> tempTransition;
    unordered_set<FAState> tempAcceptStates;

    for (FAState i = 0; i < states; i++) {

        if (isAcceptState(i)) {
            tempAcceptStates.insert(groups[i].groupNum);
        }
        for (FASymbol j = 0; j < symbols.size(); j++) {
            for (FASymbol symbol = symbols[j].first; symbol <= symbols[j].second; symbol++) {
                tempTransition[groups[i].groupNum][symbol] = groups[transition[i][symbol]].groupNum;
            }
        }
    }

    states = currentStateCount;
    startState = groups[startState].groupNum;
    acceptStates = tempAcceptStates;

    transition.clear();
    for (int i = 0; i < currentStateCount; i++) {
        transition.push_back(tempTransition[i]);
    }

    calculateTerminalStates();
}

void DFA::resetCurrentState(void)
{
    currentState = startState;
}
