//
//  BitNumber.hpp
//  Regex
//
//  Created by Min on 2024/3/17.
//

#ifndef BitNumber_hpp
#define BitNumber_hpp

#include <vector>
#include <unordered_set>

using std::vector;
using std::unordered_set;

struct BitNumber
{
private:

    const static unsigned int SegmentBits = sizeof(unsigned int) * 8;

    unsigned int segmentCount;
    unsigned int *bits;

    // set the bit of position to 0 or 1
    void set(unsigned int position, bool value);

public:

    BitNumber();
    
    // init a number whose bits are all 0 except the bit of position
    BitNumber(unsigned int position);

    // init a number whose bits are all 0 except bits of positions
    BitNumber(const vector<unsigned int>& positions);
    BitNumber(const unordered_set<unsigned int>& positions);

    BitNumber(const BitNumber &n);
    BitNumber(BitNumber &&n);

    BitNumber& operator=(const BitNumber &n);
    BitNumber& operator=(BitNumber &&n);

    ~BitNumber();

    bool operator==(const BitNumber& n) const;

    // set the bit of position to 1
    void set(unsigned int position);

    // set the bit of position to 0
    void clear(unsigned int position);

    const unsigned int hashValue() const;
};

namespace std {
    template <>
    struct hash<BitNumber> {
        std::size_t operator()(const BitNumber& n) const {
            return n.hashValue();
        }
    };
}

#endif /* BitNumber_hpp */
