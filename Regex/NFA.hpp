//
//  NFA.hpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#ifndef NFA_hpp
#define NFA_hpp

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::pair;

typedef unsigned int NFAState;
typedef unsigned int NFASymbol;
typedef pair<unsigned int, unsigned int> Substring; // pair.first means the position of string head, and pair.second means the length of string

class NFA
{

private:
    NFAState Q; // Due to all the states are numbers, we can use the count of states to represent all possible states, and the range is 0-(Q-1).
    vector<unordered_map<NFASymbol, unordered_set<NFAState>>> delta; // The transition function running like this: nextStates = delta[currentState][receivedSymbol]
    NFAState startState; // Normaly 0
    unordered_set<NFAState> acceptStates;

    const unordered_set<NFAState>& receive(const NFAState currentState, const NFASymbol symbol);
    bool isStartState(const NFAState state);
    bool isAcceptState(const NFAState state);
    bool containAcceptStates(const unordered_set<NFAState>& states);

    unordered_set<NFAState> collectEmptyCharReachableStates(unordered_set<NFAState> states);

public:
    static const NFASymbol EPSILON; // To indicate an empty symbol.

    NFA(const NFAState Q, const NFAState startState, const unordered_set<NFAState> acceptStates, vector<unordered_map<NFASymbol, unordered_set<NFAState>>> delta): Q(Q), startState(startState), acceptStates(acceptStates), delta(delta){}
    vector<Substring> parseString(const string& str);
};

#endif /* NFA_hpp */
