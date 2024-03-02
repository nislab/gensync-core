/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 12/4/23.
//

#include <GenSync/Syncs/BloomFilter.h>

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t size, int nHash)
{
    this->bfSize = size;
    this->bits.resize(size, 0);
    this->numHashes = nHash;
}

BloomFilter::BloomFilter(size_t numExpElems, float falsePosProb)
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

int BloomFilter::getNumHashes()
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

hash_t BloomFilter::_hash(const ZZ& value, long kk)
{
	hash<string> shash;

    hash_t hash_val = shash(toStr(value));
    hash_t hash_kk = shash(to_string(kk));

    return shash( to_string(hash_val) + to_string(hash_kk) );
}

void BloomFilter::insert(ZZ value)
{
    vector<int> locs;

    for(int i = 0; i < this->numHashes; i++)
	    locs.push_back(_hash(value, i) % this->bfSize);

    for (int n : locs)
        this->bits[n] = 1;
}

bool BloomFilter::exist(ZZ value)
{
    vector<int> locs;

    for(int i = 0; i < this->numHashes; i++)
        locs.push_back(_hash(value, i) % this->bfSize);

    for (int n : locs)
    {
        if(this->bits[n] == 0)
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
    size_t numBytes;
    numBytes = bits.size()/8;
    if(bits.size()%8 != 0)
        numBytes++;

    unsigned char* pp = new unsigned char[numBytes];
    int index = 0;
    vector<bool> tempBits = bits;
    reverse(tempBits.begin(), tempBits.end());

    for(size_t i = 0; i < tempBits.size(); i += 8)
    {
        unsigned char byte = 0;
        for(size_t j = 0; j < 8 && i + j < tempBits.size(); ++j)
        {
            byte |= (tempBits[i+j] ? 1 : 0) << j;
        }
        pp[index] = byte;
        index++;
    }

    ZZ res = NTL::ZZFromBytes(pp, numBytes);

    delete[] pp;

    return res;
}

BloomFilter BloomFilter::ZZtoBF(ZZ val)
{   
    size_t sz = this->getSize();
    vector<bool> resBits(sz, 0);
    
    size_t numBytes = NTL::NumBytes(val);
    unsigned char* pp = new unsigned char[numBytes];
    NTL::BytesFromZZ(pp, val, NTL::NumBytes(val));

    for(size_t i = 0; i < numBytes; ++i)
    {
        unsigned char byte = pp[i];
        for(int j = 7; j >= 0; --j)
        {
            resBits[(8*i + j)] = (byte >> j) & 1;
        }
    }

    BloomFilter res(sz, this->getNumHashes());
    reverse(resBits.begin(), resBits.end());
    res.bits = resBits;

    delete[] pp;

    return res;
}
