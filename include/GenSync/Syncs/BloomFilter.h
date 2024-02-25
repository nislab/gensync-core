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
        Builder(){
            bfSize = 0;
            numHashes = 0;
            numExpElems = 0;
            falsePosProb = 0;
        }

        /**
         * Builds a BloomFilter object.
         * setSize and setNumHashes are meant to be used together.
         * setNumExpElems and setFalsePosProb are meant to be used together.
         * If there is a mismatched use of setters, output error message and quit, returnng empty BloomFilter.
         * @return a BloomFilter object from the build parts that have been set.
         */    
        BloomFilter build()
        {
            if(bfSize != 0 && numHashes != 0)
                return BloomFilter(bfSize, numHashes);
            
            if(numExpElems != 0 && falsePosProb != 0)
                return BloomFilter(numExpElems, falsePosProb);
            
            Logger::error_and_quit("ERROR: Mismatched combination of setters used in BloomFilter construction!");
            return BloomFilter();
        }        

        /**
         * Sets the size of the BloomFilter, specifically its length in bits.
         * Meant to be used with setNumHashes setter. Cannot be used with other setters.
         * Discards all elements inserted before the resize.
         */
        Builder& setSize(size_t size)
        {
            bfSize = size;
            return *this;
        }

        /**
         * Sets the number of hash functions used by the BloomFilter for each element insertion
         * Meant to be used with setSize setter. Cannot be used with other setters.
         * Discards all elements inserted before the hashes reset.
         */
        Builder& setNumHashes(int nHash)
        {
            numHashes = nHash;
            return *this;
        }

        /**
         * Sets the number of expected elements in the BloomFilter.
         * Meant to be used with setFalsePosProb setter. Cannot be used with other setters.
         * Discards all elements inserted before the resize.
         */
        Builder& setNumExpElems(size_t numElems)
        {
            numExpElems = numElems;
            return *this;
        }

        /**
         * Sets the probability of false positives in the BloomFilter
         * Meant to be used with setNumExpElems setter. Cannot be used with other setters.
         * Discards all elements inserted before reset.
         */
        Builder& setFalsePosProb(float prob)
        {
            falsePosProb = prob;
            return *this;
        }
        
    private:
        // Length of the BloomFilter in bits
        size_t bfSize;

        // Number of hash functions used by the BloomFilter
        int numHashes;

        // Number of expected elements in BloomFilter
        size_t numExpElems;

        // The probability/rate of false positives
        float falsePosProb;
    };

    // default constructor
    BloomFilter();

    // default destructor
    ~BloomFilter();

    /**
     * Getter for size of BloomFilter, specifically its length in bits.
     * @return size_t The length of the BloomFilter's bit string
     */
    size_t getSize();

    /**
     * Getter for number of hash functions used by BloomFilter.
     * @return int The number of hash functions used by BloomFilter per element
     */
    int getNumHashes();

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
    // constructors should not be used, only builder pattern should be accessible

    /**
     * Constructs a BloomFilter object with bit string's size equal to size.
     * @param size The size of BloomFilter, specifically its length in bits
     * @param nHash The number of hash functions BloomFilter will use for each element insertion
     */
    BloomFilter(size_t size, int nHash);

    /**
     * Constructs a BloomFilter object from false positive rate as input.
     * @param numExpElems The expected number of elements in the BloomFilter
     * @param falsePosProb The rate of false positives
     */
    BloomFilter(size_t numExpElems, float falsePosProb);

    /** 
     * Returns the kk-th hash of the ZZ value.
     * @param value The value being hashed
     * @param kk The current iteration of hash(value)
     * @return hash(hash(value) + hash(kk))
     */
    static hash_t _hash(const ZZ& value, long kk);

    // all bits in BloomFilter
    vector<bool> bits;
    
    // Length of the BloomFilter in bits
    size_t bfSize;

    // number of hash functions
    int numHashes;
};

#endif
