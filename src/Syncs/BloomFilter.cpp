/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 12/4/23.
//

#include <GenSync/Syncs/BloomFilter.h>

BloomFilter::BloomFilter() = default;
BloomFilter::~BloomFilter() = default;

BloomFilter::BloomFilter(size_t size, int nHash)
{
    this->bits.resize(size, 0);
    this->numHashes = nHash;
}

BloomFilter::BloomFilter(size_t numExpElems, float falsePosProb)
{
    if(falsePosProb <= 0 || falsePosProb >= 1)
    {
        Logger::error_and_quit("ERROR: False positive probability must be set between 0 and 1 exclusive!");
    }
    
    size_t bfSize = round(-2.08 * log(falsePosProb) * numExpElems); // From "Michael Mitzenmacher. 2001. Compressed bloom filters."
    this->bits.resize(bfSize, 0);
    this->numHashes = round(-2.08 * log(falsePosProb) * log(2)); // From "Michael Mitzenmacher. 2001. Compressed bloom filters."
}

size_t BloomFilter::getSize()
{
    return this->bits.size();
}

int BloomFilter::getNumHashes() const
{
    return this->numHashes;
}

vector<bool> BloomFilter::getBits()
{
    return this->bits;
}

float BloomFilter::getFalsePosProb(size_t numExpElems)
{
    size_t bfSize = this->bits.size();
    float exp = ((float)bfSize/numExpElems) * log(2); // From "Michael Mitzenmacher. 2001. Compressed bloom filters."
    return pow(0.5, exp);
}

bf_hash_t BloomFilter::_hash(const ZZ& value, long kk)
{
	hash<string> shash;

    bf_hash_t hash_val = shash(toStr(value));
    bf_hash_t hash_kk = shash(to_string(kk));

    return shash( to_string(hash_val) + to_string(hash_kk) );
}

void BloomFilter::insert(const ZZ& value)
{
    vector<int> locs;
    size_t bfSize = this->bits.size();

    for(int i = 0; i < this->numHashes; i++)
    {
        size_t loc = _hash(value, i) % bfSize;
        this->bits[loc] = 1;
    }
}

bool BloomFilter::exist(const ZZ& value)
{
    vector<int> locs;
    size_t bfSize = this->bits.size();

    for(int i = 0; i < this->numHashes; i++)
    {
        size_t loc = _hash(value, i) % bfSize;
        if(this->bits[loc] == 0)
            return false;
    }

    return true;
}

void BloomFilter::insert(const multiset<shared_ptr<DataObject>>& tarSet)
{
    for(auto& val: tarSet)
	this->insert(val->to_ZZ());
}

string BloomFilter::toString() const
{
    string res;

    for(auto v: this->bits)
        res += to_string(v);

    return res;
}

ZZ BloomFilter::toZZ()
{
    size_t numBytes;
    int byteSize = CHAR_BIT;
    numBytes = bits.size()/byteSize;
    if(bits.size()%byteSize != 0)
        numBytes++;

    auto* pp = new unsigned char[numBytes];
    int index = 0;
    vector<bool> tempBits = bits;
    reverse(tempBits.begin(), tempBits.end());

    for(size_t i = 0; i < tempBits.size(); i += byteSize)
    {
        unsigned char byte = 0;
        for(size_t j = 0; j < byteSize && i + j < tempBits.size(); ++j)
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

BloomFilter BloomFilter::ZZtoBF(const ZZ& val)
{   
    size_t sz = this->getSize();
    vector<bool> resBits(sz, 0);
    int byteSize = CHAR_BIT;
    
    size_t numBytes = NTL::NumBytes(val);
    auto* pp = new unsigned char[numBytes];
    NTL::BytesFromZZ(pp, val, NTL::NumBytes(val));

    for(size_t i = 0; i < numBytes; ++i)
    {
        unsigned char byte = pp[i];
        for(int j = byteSize-1; j >= 0; --j)
        {
            resBits[(byteSize*i + j)] = (byte >> j) & 1;
        }
    }

    BloomFilter res(sz, this->getNumHashes());
    reverse(resBits.begin(), resBits.end());
    res.bits = resBits;

    delete[] pp;

    return res;
}
