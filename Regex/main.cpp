//
//  main.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#include <iostream>
#include "NFA.hpp"

class Solution {
public:
    bool isMatch(string s, string p) {

        NFA n{};
        NFA n1;
        NFA m{'.'};

        char t = '*';

        for (auto i = p.begin(); i <= p.end(); i++) {

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

                n.makeConcatenation(n1);
            }

            t = *i;
        }

        vector<Substring> res = n.parseString(s);

        return !res.empty() && res[0].second == s.size();
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...

//    NFA n1 = NFA(2, 0, {1}, {
//        {{'a', {1}}},
//        {},
//    });
//    NFA n2 = NFA(2, 0, {1}, {
//        {{'b', {1}}},
//        {},
//    });
//    NFA n3 = NFA(2, 0, {1}, {
//        {{'c', {1}}},
//        {},
//    });
//    NFA n4 = NFA(2, 0, {1}, {
//        {{'d', {1}}},
//        {},
//    });
//
//    vector<Substring> res1 = n1.parseString("abcddd");
//
//    n1.makeUnion(n2);
//
//    vector<Substring> res2 = n1.parseString("abcddd");
//
//    n1.makeConcatenation(n3);
//
//    vector<Substring> res3 = n1.parseString("abcddd");
//
//    n4.makeStar();
//    n1.makeConcatenation(n4);
//
//    vector<Substring> res4 = n1.parseString("abcddd");

    Solution s;
    bool a = s.isMatch("aa", "a*");

    return 0;
}
