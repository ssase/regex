//
//  main.cpp
//  Regex
//
//  Created by Min on 2024/3/8.
//

#include <iostream>
#include "NFA.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...

    NFA n1 = NFA(2, 0, {1}, {
        {{'a', {1}}},
        {{}},
    });
    NFA n2 = NFA(2, 0, {1}, {
        {{'b', {1}}},
        {{}},
    });
    NFA n3 = NFA(2, 0, {1}, {
        {{'c', {1}}},
        {{}},
    });
    NFA n4 = NFA(2, 0, {1}, {
        {{'d', {1}}},
        {{}},
    });

    vector<Substring> res1 = n1.parseString("abcd");

    n1.makeUnion(n2);

    vector<Substring> res2 = n1.parseString("abcd");

    n1.makeUnion(n3);

    vector<Substring> res3 = n1.parseString("abcd");

    n1.makeUnion(n4);

    vector<Substring> res4 = n1.parseString("abcd");


    return 0;
}
