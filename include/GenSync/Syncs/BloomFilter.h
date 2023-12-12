/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * A Bloom filter is a space-efficient probabilistic data structure which tests
 * whether an element is a member of a set. False positives are possible, but
 * not false negatives. This data structure was developed by Burton Howard Bloom in 1970.
 *
 * Created by Anish Sinha on 12/4/23.
 */

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

    /**
     * Constructs a Bloom Filter object with bit string's size equal to size.
     * @param size The size of Bloom Filter's bit string
     * @param nHash The number of hash functions Bloom Filter will use for each element
     */
    BloomFilter(size_t size, size_t nHash);

    // default destructor
    ~BloomFilter();

    /**
     * Setter for size of Bloom Filter's bit string.
     * This operation always succeeds.
     * @param size The new size of bit string
     */
    void setSize(size_t size);

    /**
     * Getter for size of Bloom Filter's bit string.
     * @return size_t The size of bit string
     */
    size_t getSize();

    /**
     * Setter for number of hash functions used by Bloom Filter.
     * @param nHash The new number of hash functions per element
     */
    void setNumHashes(size_t nHash);

    /**
     * Getter for number of hash functions used by Bloom Filter.
     * @return size_t The number of hash functions used by Bloom Filter per element
     */
    size_t getNumHashes();

    /**
     * Insert an element into Bloom Filter.
     * @param value The element to be added to Bloom Filter
     */
    void insert(ZZ value);

    /**
     * Probabilistically determine whether an element is in Bloom Filter's bit string.
     * @param value The element to be determined if in Bloom Filter
     * @return true iff element is in Bloom Filter
     */
    bool exist(ZZ value);

    /**
     * Probabilistically determine whether an element is in a Bloom Filter bit string.
     * @param value The element to be determined if in Bloom Filter
     * @param bitString The bit string to be checked
     * @return true iff element is in bit string
     */
    bool exist(ZZ value, string bitString);

    /**
     * Insert a set of elements into Bloom Filter.
     * @param tarSet target set to be added to Bloom Filter
     */
    void insert(multiset<shared_ptr<DataObject>> tarSet);

    /**
     * Convert Bloom Filter to a readable string.
     * @return string
     */
    string toString() const;

    /**
     * Convert Bloom Filter bit string to a ZZ.
     * @param bitString The bit string to be converted
     * @return ZZ
     */
    ZZ toZZ(string bitString);

    /**
     * Convert ZZ to bit string.
     * @param ZZ The ZZ to be converted
     * @return string
     */
    string ZZtoBitString(ZZ val);

protected:
    // local data

    // Returns the kk-th unique hash of the zz. From IBLT.h.
    static hash_t _hashK(const ZZ &item, long kk);
    static hash_t _hash(const hash_t& initial, long kk);

    // all bits in bloom filter
    string bits;
    
    // size of bloom filter
    size_t bfSize;

    // number of hash functions
    size_t numHashes;
};

#endif
