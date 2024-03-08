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

    NFA n = NFA(2, 0, {1}, {
        {{'a', {1}}},
        {{}},
    });

    vector<Substring> res = n.parseString("aabagdfgdfgafgdfgdgadsfgdfgdas");

    return 0;
}
