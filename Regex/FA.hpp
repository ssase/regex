//
//  FA.hpp
//  Regex
//
//  Created by Min on 2024/3/13.
//

#ifndef FA_hpp
#define FA_hpp

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::pair;

namespace FAS
{

typedef unsigned int FAState;
typedef unsigned int FASymbol;
typedef pair<unsigned int, unsigned int> Substring; // pair.first means the position of string head, and pair.second means the length of string

const static FAState StateNotFound = -1;

class FA {

protected:
    FAState states; // Due to all the states are numbers, we can use the count of states to represent all possible states, and the range is [0, states-1].
    vector<pair<FASymbol, FASymbol>> symbols; // All symbols the FA can receive, here we store ranges of symbols for saving space. for example, if you want symbols to be a-z, you can put pair('a', 'z') into it.
    FAState startState; // Normaly 0
    unordered_set<FAState> acceptStates; // Make sure being different from each other

    virtual void simplify(void);
    virtual void completeChanging(void);

public:
    
    FA();
    FA(const FAState states, const FAState startState, const unordered_set<FAState>& acceptStates);
    FA(const FAState states, const vector<pair<FASymbol, FASymbol>>& symbols, const FAState startState, const unordered_set<FAState>& acceptStates);

    virtual void receive(const FASymbol symbol);
    virtual vector<Substring> recognize(const string& str);

    bool isSymbolInRange(const FASymbol symbol) const;
    bool isStartState(const FAState state) const;
    bool isAcceptState(const FAState state) const;
    bool containAcceptStates(const unordered_set<FAState>& states) const;

//    virtual void makeUnion(const FA& n);
//    virtual void makeConcatenation(const FA& n);
//    virtual void makeStar(void);

};

}

#endif /* FA_hpp */
