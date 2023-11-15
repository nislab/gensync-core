//#include <GenSync/Syncs/BloomFilter.h>
#include "../../include/GenSync/Syncs/BloomFilter.h"

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t size, size_t nHash)
{
    this->setSize(size);
    this->setNumHashes(nHash);
}

void BloomFilter::setSize(size_t size)
{
    this->bfSize = size;
    this->bits.clear();

    for (int i = 0; i < this->bfSize; i++)
        this->bits.push_back(0);
}

size_t BloomFilter::getSize()
{
    return this->bfSize;
}

void BloomFilter::setNumHashes(size_t nHash)
{
    this->numHashes = nHash;
}

size_t BloomFilter::getNumHashes()
{
    return this->numHashes;
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
    vector<int> locs;

    for(int i = 0; i < this->numHashes; i++)
	locs.push_back(_hashK(value, i) % this->bfSize);

    for (int n : locs)
        this->bits[n] = 1;
}

bool BloomFilter::exist(ZZ value)
{
    vector<int> locs;

    for(int i = 0; i < this->numHashes; i++)
        locs.push_back(_hashK(value, i) % this->bfSize);

    for (int n : locs)
    {
	if(this->bits[n] == 0)
	{
		return false;
	}		
    }

    return true;
}

void BloomFilter::insert(multiset<shared_ptr<DataObject>> tarSet, size_t expnChldSet)
{
    for(auto& val: tarSet)
	this->insert(val->to_ZZ());
}

string BloomFilter::toString() const
{
    string str = "";
    for (bool b : this->bits)
        str += to_string(b);

    return str;
}
