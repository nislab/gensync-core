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

    BloomFilter(size_t expectedNumEntries);

    // default destructor
    ~BloomFilter();

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
     * @return the number of bits in the BloomFilter. Not necessarily equal to the expected number of entries
     */
    size_t size() const;

    /**
     * @return the size of a value stored in the BloomFilter.
     */
    size_t eltSize() const;

    vector<bool> bits; /* vector for all bits in BloomFilter */

protected:
    // local data

    // default constructor - no internal parameters are initialized
    BloomFilter();

    // the value size, in bits
    size_t valueSize;
};

#endif
