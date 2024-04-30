/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Thomas Poimenidis on 4/12/24
//

#ifndef GENSYNCLIB_GENIBLT_H
#define GENSYNCLIB_GENIBLT_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include <functional>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Data/DataObject.h>

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

// Shorthand for the hash type
typedef unsigned long int hash_t;

/**
 * GenIBLT (Generalized Invertible Bloom Lookup Table)
 */
class GenIBLT
{
public:

    /**
     * Builder pattern for creating IBLT objects
     */
    class Builder
    {
    public:
        /** Constructor for builder pattern */
        Builder()
        {
            calcNumHashes = NULL;
            numHashes = 0;
            numHashCheck = 0;
            numCells = 0;
            expectedNumEntries = 0;
            valueSize = 0;
        }

        /**
         * Builds a GenIBLT object.
         * If there is an invalid field for GenIBLT construction, output error message and quit.
         * @return a GenIBLT object from the build parts that have been set.
         */
        GenIBLT build()
        {
            if (numHashes > 0 && numHashCheck > 0 && expectedNumEntries > 0 && valueSize > 0) {
                return GenIBLT(numHashes, numHashCheck, expectedNumEntries, valueSize);
            } else if (numCells > 0 && valueSize > 0 && calcNumHashes != NULL) {
                return GenIBLT(numCells, valueSize, calcNumHashes);
            } 
            
            Logger::error_and_quit("ERROR: Invalid Combination of Parameters for GenIBLT Construction");
            return GenIBLT();      
        }

        /**
         * Sets the calcNumHashes functional for the GenIBLT to be built.
         * @return The updated Builder which includes the calcNumHashes specification
         */
        Builder& setCalcNumHashes(function<long(ZZ)> calcNumHashes)
        {
            this->calcNumHashes = calcNumHashes;
            return *this;
        }

        /**
         * Sets the numHashes for the GenIBLT to be built.
         * @return The updated Builder which includes the numHashes specification
         */
        Builder& setNumHashes(long numHashes)
        {
            this->numHashes = numHashes;
            return *this;
        }

        /**
         * Sets the numHashCheck for the GenIBLT to be built.
         * @return The updated Builder which includes the numHashCheck specification.
         */
        Builder& setNumHashCheck(long numHashCheck)
        {
            this->numHashCheck = numHashCheck;
            return *this;
        }

        /**
         * Sets the numCells for the GenIBLT to be built.
         * @return The updated Builder which includes the numCells specification.
         */
        Builder& setNumCells(size_t numCells)
        {
            this->numCells = numCells;
            return *this;
        }

        /**
         * Sets the expectedNumEntries for the GenIBLT to be built.
         * @return The updated Builder which includes the expectedNumEntries specification.
         */
        Builder& setExpectedNumEntries(size_t expectedNumEntries)
        {
            this->expectedNumEntries = expectedNumEntries;
            return *this;
        }

        /**
         * Sets the valueSize for the GenIBLT to be built.
         * @return The updated Builder which includes the valueSize specification.
         */
        Builder& setValueSize(size_t valueSize)
        {
            this->valueSize = valueSize;
            return *this;
        }

    private:
        // The functional to calculate the number of hashes
        function<long(ZZ)> calcNumHashes;

        // The number of hashes used per insert
        long numHashes;

        // The number hash used to create hash-check for each entry
        long numHashCheck;

        // The exact number of cells for IBLT
        size_t numCells;

        // The expected amount of entries to be placed into the IBLT
        size_t expectedNumEntries;

        // The size of the values being added, in bits
        size_t valueSize;
    };

    // Communicant needs to access the internal representation of an IBLT to send and receive it
    friend class Communicant;

    // default constructor
    GenIBLT();

    // default destructor
    ~GenIBLT();

    /**
     * Inserts a key-value pair to the IBLT.
     * This operation always succeeds.
     * @param key The key to be added
     * @param value The value to be added
     * @require The key must be distinct in the IBLT
     */
    void insert(ZZ key, ZZ value);
    
    /**
     * Erases a key-value pair from the IBLT.
     * This operation always succeeds.
     * @param key The key to be removed
     * @param value The value to be removed
     */
    void erase(ZZ key, ZZ value);
    
    /**
     * Produces the value s.t. (key, value) is in the IBLT.
     * This operation doesn't always succeed.
     * This operation is destructive, as entries must be "peeled" away in order to find an element, i.e.
     * entries with only one key-value pair are subtracted from the IBLT until (key, value) is found.
     * @param key The key corresponding to the value returned by this function
     * @param result The resulting value corresponding with the key, if found.
     * If not found, result will be set to 0. result is unchanged iff the operation returns false.
     * @return true iff the presence of the key could be determined
     */
    bool get(ZZ key, ZZ& result);
    
    /**
     * Produces a list of all the key-value pairs in the IBLT.
     * With a low, constant probability, only partial lists will be produced
     * Listing is destructive, as the same peeling technique used in the get method is used.
     * Will remove all key-value pairs from the IBLT that are listed.
     * @param positive All the elements that could be inserted.
     * @param negative All the elements that were removed without being inserted first.
     * @return true iff the operation has successfully recovered the entire list
     */
    bool listEntries(vector<pair<ZZ, ZZ>>& positive, vector<pair<ZZ, ZZ>>& negative);
    
    /**
     * Insert a set of elements into IBLT
     * @param tarSet target set to be added to IBLT
     * @param elemSize size of element in the set
     * @param expnChldSet expected number of elements in the target set
    */
    void insert(multiset<shared_ptr<DataObject>> tarSet, size_t elemSize, size_t expnChldSet);

    /**
     * Delete a set of elements from IBLT
     * @param tarSet the target set to be deleted
     * @param elemSize size of element in the chld set
     * @param expnChldSet expected number of elements in the target set
    */
    void erase(multiset<shared_ptr<DataObject>> tarSet, size_t elemSize, size_t expnChldSet);

    /**
     * Convert IBLT to a readable string
     * @return string
    */
    string toString() const;

    /**
     * fill the hashTable with a string generated from IBLT.toString() function
     * @param inStr a readable ascii string generted from IBLT.toString() function
    */
    void reBuild(string &inStr);

    /**
     * Subtracts two IBLTs.
     * -= is destructive and assigns the resulting iblt to the lvalue, whereas - isn't. -= is more efficient than -
     * @param other The IBLT that will be subtracted from this IBLT
     * @require IBLT must have the same number of entries and the values must be of the same size
     */
    GenIBLT operator-(const GenIBLT& other) const;
    GenIBLT& operator-=(const GenIBLT& other);

    /**
     * @return the functional that calculates the number of hashes for each element
     */
    function<long(ZZ)> getCalcNumHashes() const;

    /**
     * @return the number of cells in the IBLT. Not necessarily equal to the expected number of entries
     */
    size_t size() const;

    /**
     * @return the size of a value stored in the IBLT.
     */
    size_t eltSize() const;

    vector<hash_t> hashes; /* vector for all hashes of sets */

protected:
    // constructors should not be used, only builder pattern should be accessible.

    /**
     * Constructs a GenIBLT object with size relative to expectedNumEntries.
     * @param numHashes The number of hashes used per insert
     * @param numHashCheck The number hash used to create hash-check for each entry.
     * @param expectedNumEntries The expected amount of entries to be placed into the IBLT
     * @param valueSize The size of the values being added, in bits
     */
    GenIBLT(long numHashes, long numHashCheck, size_t expectedNumEntries, size_t valueSize);
    
    /**
     * Constructs a GenIBLT object with size numCells
     * @param numCells The number of cells in the IBLT
     * @param valueSize The size of the values being added, in bits
     * @param calcNumHashes The functional to calculate the number of hashes
     */
    GenIBLT(size_t numCells, size_t valueSize, function<long(ZZ)> calcNumHashes);

    // Helper function for insert and erase
    void _insert(long plusOrMinus, ZZ key, ZZ value);

    // Returns the kk-th unique hash of the zz that produced initial.
    static hash_t _hashK(const ZZ &item, long kk);
    static hash_t _hash(const hash_t& initial, long kk);
    static hash_t _setHash(multiset<shared_ptr<DataObject>> &tarSet);

    /* Insert an IBLT together with a value into a bigger IBLT
    * @param chldIBLT the IBLT to be inserted
    * @param chldHash a value represent in the hash_t type
    * */
    void insert(GenIBLT &chldIBLT, hash_t &chldHash);

    /* Erase an IBLT together with a value into a bigger IBLT
    * @param chldIBLT the IBLT to be erased
    * @param chldHash a value represent in the hash_t type
    * */
    void erase(GenIBLT &chldIBLT, hash_t &chldHash);

    // Represents each entry in the iblt
    class HashTableEntry
    {
    public:
        // Net insertions and deletions that mapped to this cell
        long count;

        // The bitwise xor-sum of all keys mapped to this cell
        ZZ keySum;

        // The bitwise xor-sum of all keySum checksums at each allocation
        hash_t keyCheck;

        // The bitwise xor-sum of all values mapped to this cell
        ZZ valueSum;

        // Returns whether the {numHashCheck} entry contains just one insertion or deletion
        bool isPure(long numHashCheck) const;

        // Returns whether the entry is empty
        bool empty() const;
    };

    // The number of hashes used per insert
    long numHashes;

    // Functional that calculates the number of hashes for each element.
    function<long(ZZ)> calcNumHashes;

    // The number hash used to create the hash-check for each entry
    long numHashCheck;

    // vector of all entries
    vector<HashTableEntry> hashTable;

    // The size of the values being added, in bits
    size_t valueSize;
};

#endif //GENSYNCLB_GENIBLT_H