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
    unordered_set<FAState> terminalStates = unordered_set<FAState>(); // for any t in terminalStates and all s in symbols, transition[t][s] == t

protected:

    FAState transitResult(FAState state, FASymbol symbol);
    bool isTerminalState(FAState state);
    void calculateTerminalStates(void);

    void simplify(void) override;
    void completeChanging(void) override;

public:

    DFA();
    DFA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition);
    DFA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, FAState>>& transition);

    //    DFA(const FASymbol specialSymbol);
    //    DFA(const string pattern); // Init a DFA with a regex
    DFA(const NFA& n);

    void receive(const FASymbol symbol) override;
    vector<Substring> recognize(const string& str) override;

    void makeUnion(const DFA& n);
    void makeConcatenation(const DFA& n);
    void makeStar(void);

};

}

#endif /* DFA_hpp */
