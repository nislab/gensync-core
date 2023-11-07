//#include <GenSync/Syncs/BloomFilter.h>
#include "../../include/GenSync/Syncs/BloomFilter.h"

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t expectedNumEntries)
{
    // 1.5x expectedNumEntries gives very low probability of decoding failure
    size_t nSize = expectedNumEntries + expectedNumEntries / 2;
    this->bfSize = nSize;

    this->bits.clear();
    for (int i = 0; i < nSize; i++)
        this->bits.push_back(0);
}

hash_t BloomFilter::_hash(const hash_t& initial, long kk)
{
	if(kk == -1) return initial;
	std::hash<std::string> shash;
	return _hash(shash(toStr(initial)), kk-1);
}

hash_t BloomFilter::_hashK(const ZZ &item, long kk)
{
	std::hash<std::string> shash;
	return _hash(shash(toStr(item)), kk-1);
}

void BloomFilter::insert(ZZ value)
{
    int hashFuncs = 3;
    vector<int> locs;

    for(int i = 0; i < hashFuncs; i++)
	locs.push_back(_hashK(value, i) % this->bfSize);

    for (int n : locs)
        this->bits[n] = 1;
}

size_t BloomFilter::size() const
{
    return this->bits.size();
}

string BloomFilter::toString() const
{
    string str = "";
    for (bool b : this->bits)
        str += to_string(b);

    return str;
}
