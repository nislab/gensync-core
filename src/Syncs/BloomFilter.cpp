/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 12/4/23.
//

#include <GenSync/Syncs/BloomFilter.h>

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t size, size_t nHash)
{
    this->bfSize = size;
    this->bits.resize(size, 0);
    this->numHashes = nHash;
}

BloomFilter::BloomFilter(size_t numExpElems, float falsePosProb, bool use)
{
    if(falsePosProb < 0 || falsePosProb > 1)
    {
        falsePosProb = 0.05;
    }
    
    this->bfSize = round(-2.08 * log(falsePosProb) * numExpElems);
    this->bits.resize(this->bfSize, 0);
    this->numHashes = round(-2.08 * log(falsePosProb) * log(2));
}

size_t BloomFilter::getSize()
{
    return this->bfSize;
}

size_t BloomFilter::getNumHashes()
{
    return this->numHashes;
}

vector<bool> BloomFilter::getBits()
{
    return this->bits;
}

float BloomFilter::getFalsePosProb(size_t numExpElems)
{
    float exp = ((float)bfSize/numExpElems) * log(2);
    return pow(0.5, exp);
}

void BloomFilter::setBits(vector<bool> bitString)
{
    if(this->getSize() == bitString.size())
        this->bits = bitString;
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
    string res = "";

    for(auto v: this->bits)
        res += to_string(v);

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

BloomFilter BloomFilter::ZZtoBF(ZZ val)
{
    vector<bool> resBits;
    size_t sz = this->getSize();

    for(int i = 0; i < sz; i++)
    {
        ZZ p2;
        power(p2, 2, sz-i-1);
        if(p2 <= val)
        {
            resBits.push_back(1);
            val -= p2;
        }
        else
        {
            resBits.push_back(0);
        }
    }

    BloomFilter res(sz, this->getNumHashes());
    res.setBits(resBits);

    return res;
}
