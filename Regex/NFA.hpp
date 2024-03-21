//
//  NFA.hpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#ifndef NFA_hpp
#define NFA_hpp

#include "FA.hpp"

namespace FAS
{

class DFA;

class NFA: public FA
{
private:

    static const FASymbol EPSILON; // To indicate an empty symbol.

    unordered_set<FAState> currentStates;
    vector<unordered_map<FASymbol, unordered_set<FAState>>> transition;

    unordered_set<FAState> collectEmptySymbolReachableStates(unordered_set<FAState> states) const;

    // Although there is only one start state, but we should also consider ε(aka empty string), this will help set currentStates with startState after considering empty string reachability.
    void calculateCurrentStates(void);

protected:

    const unordered_set<FAState> transitResult(FAState state, FASymbol symbol) const;
    const unordered_set<FAState> transitResult(unordered_set<FAState> states, FASymbol symbol) const;

    void simplify(void) override;
    void completeChanging(void) override;

public:

    friend class DFA;

    NFA();
    NFA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, unordered_set<FAState>>>& transition);
    NFA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates, const vector<unordered_map<FASymbol, unordered_set<FAState>>>& transition);
    NFA(const FASymbol specialSymbol);
    // Init an NFA with a regex
    NFA(const string pattern);

    NFA(const DFA& d);

    void receive(const FASymbol symbol) override;
    virtual vector<Substring> recognize(const string& str) override;

    void makeUnion(const NFA& n);
    void makeConcatenation(const NFA& n);
    void makeStar(void);

};

}
#endif /* NFA_hpp */
