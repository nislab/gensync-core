#ifndef GENSYNCLIB_BLOOMFILTER_H
#define GENSYNCLIB_BLOOMFILTER_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Data/DataObject.h>

using std::hash;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;
using namespace NTL;

// Shorthand for the hash type
typedef unsigned long int hash_t;

class BloomFilter
{
public:
    // Communicant needs to access the internal representation
    friend class Communicant;

    // default constructor
    BloomFilter();

    BloomFilter(size_t size, size_t nHash);

    // default destructor
    ~BloomFilter();

    void setSize(size_t size);
    size_t getSize();

    void setNumHashes(size_t nHash);
    size_t getNumHashes();

    static hash_t _hashK(const ZZ &item, long kk);
    static hash_t _hash(const hash_t& initial, long kk);

    void insert(ZZ value);

    /**
     * Insert a set of elements into BloomFilter
     * @param tarSet target set to be added to BloomFilter
     * @param elemSize size of element in the set
     * @param expnChldSet expected number of elements in the target set
     */
    void insert(multiset<shared_ptr<DataObject>> tarSet, size_t expnChldSet);

    /**
     * Convert BloomFilter to a readable string
     * @return string
     */
    string toString() const;

    /**
     * @return the size of a value stored in the BloomFilter.
     */
    size_t eltSize() const; 

protected:
    // local data

    // all bits in bloom filter
    vector<bool> bits;

    // the value size, in bits
    size_t valueSize;
    
    // size of bloom filter
    size_t bfSize;

    // number of hash functions
    size_t numHashes;
};

#endif
