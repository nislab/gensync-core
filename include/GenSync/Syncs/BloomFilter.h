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

    /**
     * Builder pattern for creating BloomFilter objects
     */
    class Builder
    {
    public:

        /** Constructor for builder pattern */
        Builder(){}

        /**
         * Builds a BloomFilter object.
         * @return a BloomFilter object from the build parts that have been set.
         */    
        BloomFilter build()
        {
            return BloomFilter(bfSize, numHashes);
        }        

        /**
         * Sets the size of the BloomFilter, specifically its length in bits.
         * Discards all elements inserted before the resize.
         */
        Builder& setSize(size_t size)
        {
            bfSize = size;
            return *this;
        }

        /**
         * Sets the number of hash functions used by the BloomFilter for each element insertion
         * Discards all elements inserted before the hashes reset.
         */
        Builder& setNumHashes(size_t nHash)
        {
            numHashes = nHash;
            return *this;
        }
        
    private:

        // Length of the BloomFilter in bits
        size_t bfSize;

        // Number of hash functions used by the BloomFilter
        size_t numHashes;
    };
    
    // default constructor
    BloomFilter();

    /**
     * Constructs a BloomFilter object with bit string's size equal to size.
     * @param size The size of BloomFilter, specifically its length in bits
     * @param nHash The number of hash functions BloomFilter will use for each element insertion
     */
    BloomFilter(size_t size, size_t nHash);

    /**
     * Constructs a BloomFilter object from false positive rate as input.
     * @param numExpElems The expected number of elements in the BloomFilter
     * @param falsePosProb The rate of false positives
     * @param use Additional parameter meant to differentiate the two constructors, not used in the function
     */
    BloomFilter(size_t numExpElems, float falsePosProb, bool use);

    // default destructor
    ~BloomFilter();

    /**
     * Getter for size of BloomFilter, specifically its length in bits.
     * @return size_t The length of the BloomFilter's bit string
     */
    size_t getSize();

    /**
     * Getter for number of hash functions used by BloomFilter.
     * @return size_t The number of hash functions used by BloomFilter per element
     */
    size_t getNumHashes();

    /**
     * Getter for BloomFilter's bits in form of vector<bool>.
     * @return vector<bool> The bit string
     */
    vector<bool> getBits();

    /**
     * Calculates BloomFilter's rate of false positives.
     * @param numExpElems The expected number of elements stored in the BloomFilter
     * @return float The false positive probability/rate
     */
    float getFalsePosProb(size_t numExpElems);

    /**
     * Insert an element into BloomFilter.
     * @param value The element to be added to BloomFilter
     */
    void insert(ZZ value);
    
    /**
     * Insert a set of elements into BloomFilter.
     * @param tarSet target set to be added to BloomFilter
     */
    void insert(multiset<shared_ptr<DataObject>> tarSet);

    /**
     * Probabilistically determine whether an element is in BloomFilter's bit string.
     * @param value The element to be determined if in BloomFilter
     * @return true iff element is probably in BloomFilter
     */
    bool exist(ZZ value);

    /**
     * Probabilistically determine whether an element is in a BloomFilter bit string.
     * @param value The element to be determined if in BloomFilter
     * @param bitString The bit string to be checked
     * @return true iff element is probably in bit string
     */
    bool exist(ZZ value, vector<bool> bitString);

    /**
     * Convert BloomFilter to a readable string.
     * @return string
     */
    string toString() const;

    /**
     * Convert BloomFilter to a ZZ.
     * @return ZZ
     */
    ZZ toZZ();

    /**
     * Convert ZZ to BloomFilter.
     * @param ZZ The ZZ to be converted
     * @return BloomFilter
     */
    BloomFilter ZZtoBF(ZZ val);

protected:
    // local data

    /**
     * Setter for BloomFilter's bit string.
     * @param bitString The new bit string
     */
    void setBits(vector<bool> bitString);

    // Returns the kk-th hash of the ZZ. From IBLT.h.
    static hash_t _hashK(const ZZ &item, long kk);
    static hash_t _hash(const hash_t& initial, long kk);

    // all bits in BloomFilter
    vector<bool> bits;
    
    // Length of the BloomFilter in bits
    size_t bfSize;

    // number of hash functions
    size_t numHashes;
};

#endif
