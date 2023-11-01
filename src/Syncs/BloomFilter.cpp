#include <GenSync/Syncs/BloomFilter.h>

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t expectedNumEntries)
{
    // 1.5x expectedNumEntries gives very low probability of decoding failure
    size_t nSize = expectedNumEntries + expectedNumEntries / 2;

    this->bits.clear();
    for (int i = 0; i < nSize; i++)
        this->bits.push_back(0);
}

void BloomFilter::insert(ZZ value)
{
    vector<int> locs = hashes(value);
    for (int n : locs)
        this->bits[n] = 1;
}

size_t BloomFilter::size()
{
    return this->bits.size();
}

string BloomFilter::toString()
{
    string str = "";
    for (bool b : this->bits)
        str += b;

    return str;
}