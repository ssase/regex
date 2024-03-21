//
//  main.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//


#include <iostream>

#include "BitNumber.hpp"
#include "NFA.hpp"
#include "DFA.hpp"

using namespace FAS;

vector<int> test()
{
    return {};
}

class Solution {
public:
    bool isMatch(string s, string p) {

        NFA n = NFA(p);
        vector<Substring> res = n.recognize(s);

        return !res.empty() && res[0].second == s.size();
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...

    NFA n1 = NFA(2, {{'a', 'b'}}, 0, {1}, {
        {{'a', {1}}},
        {},
    });

    NFA n2 = NFA(2, {{'a', 'b'}}, 0, {1}, {
        {{'b', {1}}},
        {},
    });

    vector<Substring> res1 = n1.recognize("abcddd");

    n1.makeUnion(n2);

    vector<Substring> res2 = n1.recognize("abcddd");

    n1.makeConcatenation(n2);

    vector<Substring> res3 = n1.recognize("abcddd");

    n2.makeStar();
    n1.makeConcatenation(n2);

    vector<Substring> res4 = n1.recognize("abcddd");

//    Solution s;
//    bool a = s.isMatch("abb", "bbb*");



    DFA d(n1);

    auto res21 = d.recognize("aa");

    return 0;
}
