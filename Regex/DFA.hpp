//
//  DFA.hpp
//  Regex
//
//  Created by Min on 2024/3/12.
//

#ifndef DFA_hpp
#define DFA_hpp

#include "HashValue.h"
#include "FA.hpp"

namespace FAS
{

class NFA;

class DFA: public FA
{
private:

    friend class NFA;

    FAState currentState;
    vector<unordered_map<FASymbol, FAState>> transition;
    unordered_set<FAState> terminalStates = {}; // for any t in terminalStates and all s in symbols, transition[t][s] == t

    void resetCurrentState(void);

protected:

    FAState transitResult(FAState state, FASymbol symbol);
    bool isTerminalState(FAState state);
    void calculateTerminalStates(void);

    void simplify(void) override;

public:

    DFA();
    DFA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition);
    DFA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition);

    //    DFA(const FASymbol specialSymbol);
    //    DFA(const string pattern); // Init a DFA with a regex
    DFA(const NFA& n);

    // Before using this, make sure the `currentState` is what you need. Call `resetCurrentState` if you want to begin from `startState`.
    void receive(const FASymbol symbol) override;
    bool recognize(const string& str) override;
    vector<Substring> findRecognizedSubstrings(const string& str) override;

    void makeUnion(const DFA& n);
    void makeConcatenation(const DFA& n);
    void makeStar(void);

};

}

#endif /* DFA_hpp */
