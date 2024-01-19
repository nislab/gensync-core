/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 12/4/23.
//

#include <GenSync/Syncs/BloomFilter.h>

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
    this->bits.resize(size, 0);
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
    return exist(value, this->bits);
}

bool BloomFilter::exist(ZZ value, vector<bool> bitString)
{
    vector<int> locs;

    if(bitString.size() != this->bfSize)
	    return false;

    for(int i = 0; i < this->numHashes; i++)
        locs.push_back(_hashK(value, i) % this->bfSize);

    for (int n : locs)
    {
        if(bitString[n] == 0)
                return false;
    }

    return true;
}

void BloomFilter::insert(multiset<shared_ptr<DataObject>> tarSet)
{
    for(auto& val: tarSet)
	this->insert(val->to_ZZ());
}

string BloomFilter::toString() const
{
    string res(this->bits.begin(), this->bits.end());
    return res;
}

ZZ BloomFilter::toZZ()
{
    ZZ res;
    size_t sz = this->getSize();

    for(int i = 0; i < sz; i++)
    {
        if(this->bits[sz - i - 1] == 1)
        {
            ZZ p2;
            power(p2, 2, i);
            res += p2;
        }
    }

    return res;
}

vector<bool> BloomFilter::ZZtoBitString(ZZ val)
{
    vector<bool> res;
    size_t sz = this->getSize();

    for(int i = 0; i < sz; i++)
    {
        ZZ p2;
        power(p2, 2, sz-i-1);
        if(p2 <= val)
        {
            res.push_back(1);
            val -= p2;
        }
        else
        {
            res.push_back(0);
        }
    }

    return res;
}
