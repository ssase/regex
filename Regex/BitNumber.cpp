//
//  BitNumber.cpp
//  Regex
//
//  Created by Min on 2024/3/17.
//

#include "HashValue.h"
#include "BitNumber.hpp"

BitNumber::BitNumber(): segmentCount(0), bits(nullptr) {}

BitNumber::BitNumber(unsigned int position): BitNumber()
{
    set(position);
}

BitNumber::BitNumber(const vector<unsigned int>& positions): BitNumber()
{
    for (auto pos : positions) {
        set(pos);
    }
}

BitNumber::BitNumber(const unordered_set<unsigned int>& positions): BitNumber()
{
    for (auto pos : positions) {
        set(pos);
    }
}

BitNumber::BitNumber(const BitNumber &n)
{
    if (this == &n) { return; }
    segmentCount = n.segmentCount;
    if (segmentCount > 0) {
        bits = new unsigned int[segmentCount];
        memcpy(bits, n.bits, segmentCount * sizeof(unsigned int));
    } else {
        bits = nullptr;
    }
}

BitNumber::BitNumber(BitNumber &&n)
{
    if (this == &n) {
        assert(1);
        return;
    }

    segmentCount = n.segmentCount;
    bits = n.bits;
    n.segmentCount = 0;
    n.bits = nullptr;
}

BitNumber& BitNumber::operator=(const BitNumber &n)
{
    if (this == &n) { return *this; }
    segmentCount = n.segmentCount;
    if (bits) {
        delete[] bits;
    }
    if (segmentCount > 0) {
        bits = new unsigned int[segmentCount];
        memcpy(bits, n.bits, segmentCount * sizeof(unsigned int));
    } else {
        bits = nullptr;
    }
    return *this;
}

BitNumber& BitNumber::operator=(BitNumber &&n)
{
    if (this == &n) {
        assert(1);
        return *this;
    }

    segmentCount = n.segmentCount;
    if (bits) {
        delete[] bits;
    }
    bits = n.bits;
    n.segmentCount = 0;
    n.bits = nullptr;
    return *this;
}

BitNumber::~BitNumber()
{
    if (bits) {
        delete[] bits;
    }
}

bool BitNumber::operator==(const BitNumber& n) const
{
    if (segmentCount == n.segmentCount) {
        for (int i = 0; i < segmentCount; i++) {
            if (bits[i] != n.bits[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// set the bit of position to 1
void BitNumber::set(unsigned int position)
{
    set(position, 1);
}

// set the bit of position to 0
void BitNumber::clear(unsigned int position)
{
    set(position, 0);
}

void BitNumber::set(unsigned int position, bool value)
{
    unsigned int index = position / SegmentBits;
    unsigned int pos = position % SegmentBits;

    if (index >= segmentCount) {
        unsigned int *data = new unsigned int[index + 1]();
        if (bits) {
            memcpy(data, bits, segmentCount * sizeof(unsigned int));
            delete [] bits;
        }
        segmentCount = index + 1;
        bits = data;
        data = nullptr;
    }

    if (value) {
        bits[index] |= 1 << pos;
    } else {
        bits[index] &= ~(1 << pos);
    }
}

const unsigned int BitNumber::hashValue() const
{
    unsigned int res = 0;
    if (segmentCount) {
        res ^= bits[0];
        if (segmentCount > 1) {
            res ^= bits[segmentCount - 1];
        }
    }
    return res;
}
