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
    this->bits = "";

    for (int i = 0; i < this->bfSize; i++)
        this->bits += '0';
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
        this->bits[n] = '1';
}

bool BloomFilter::exist(ZZ value)
{
    return exist(value, this->bits);
}

bool BloomFilter::exist(ZZ value, string bitString)
{
    vector<int> locs;

    if(bitString.length() != this->bfSize)
	    return false;

    for(int i = 0; i < this->numHashes; i++)
        locs.push_back(_hashK(value, i) % this->bfSize);

    for (int n : locs)
    {
        if(bitString[n] == '0')
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
    return this->bits;
}

ZZ BloomFilter::toZZ(string bitString)
{
    ZZ res;
    size_t sz = this->getSize();

    for(int i = 0; i < sz; i++)
    {
        if(this->bits[sz - i - 1] == '1')
            res += pow(2, i);
    }

    return res;
}

string BloomFilter::ZZtoBitString(ZZ val)
{
    string res = "";
    size_t sz = this->getSize();

    for(int i = 0; i < sz; i++)
    {
        if(pow(2, sz-i-1) <= val)
        {
            res += '1';
            val -= pow(2,sz-i-1);
        }
        else
        {
            res += '0';
        }
    }

    return res;
}
