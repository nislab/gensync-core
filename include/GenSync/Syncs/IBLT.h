/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18.
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research.
//

#ifndef GENSYNCLIB_IBLT_H
#define GENSYNCLIB_IBLT_H

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Data/DataObject.h>
#include <GenSync/Syncs/GenIBLT.h>

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

// Shorthand for the hash type
typedef unsigned long int hash_t;

/*
 * IBLT (Invertible Bloom Lookup Table) is a data-structure designed to add
 * probabilistic invertibility to the standard Bloom Filter data-structure.
 *
 * A complete description of the IBLT data-structure can be found in: 
 * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 
 * arXiv preprint arXiv:1101.2245 (2011).
 */
class IBLT : public GenIBLT
{
public:

    /**
     * Builder pattern for creating GenIBLT objects 
     */
    class Builder
    {
    public:
        /** Constructor for builder pattern */
        Builder()
        {
            numHashes = 0;
            numHashCheck = 0;
            expectedNumEntries = 0;
            valueSize = 0;
        }

        /**
         * Builds an IBLT object.
         * If there is an invalid field for GenIBLT construction, output error message and quit.
         * @return an IBLT object from the build parts that have been set 
         */
        IBLT build()
        {
            return IBLT(numHashes, numHashCheck, expectedNumEntries, valueSize);
        }

        /**
         * Sets the numHashes for the GenIBLT to be built.
         * @return the updated Builder which includes the numHashes specification
         */
        Builder& setNumHashes(long numHashes)
        {
            this->numHashes = numHashes;
            return *this;
        }

        /**
         * Sets the numHashCheck for the IBLT to be built.
         * @return the updated Builder which includes the numHashCheck specification.
         */
        Builder& setNumHashCheck(long numHashCheck)
        {
            this->numHashCheck = numHashCheck;
            return *this;
        }

        /**
         * Sets the expectedNumEntries for the IBLT to be built.
         * @return the updated Builder which includes the expectedNumEntries specification.
         */
        Builder& setExpectedNumEntries(size_t expectedNumEntries)
        {
            this->expectedNumEntries = expectedNumEntries;
            return *this;
        }

        /**
         * Sets the valueSize for the IBLT to be built.
         * @return the updated Builder which includes the valueSize specification.
         */
        Builder& setValueSize(size_t valueSize)
        {
            this->valueSize = valueSize;
            return *this;
        }

    private:
        // The number of hashes used per insert
        long numHashes;

        // The number hash used to create hash-check for each entry
        long numHashCheck;

        // The expected amount of entries to be placed into the IBLT
        size_t expectedNumEntries;

        // The size of the values being added, in bits
        size_t valueSize;
    };

    // Communicant needs to access the internal representation of an IBLT to send and receive it
    friend class Communicant;

    // default constructor
    IBLT();

    // default destructor
    ~IBLT();

public:
    // constructors should not be used, only builder pattern should be accessible.

    /**
     * Constructs an IBLT object with size relative to expectedNumEntries.
     * @param numHashes The number of hashes used per insert
     * @param numHashCheck The number hash used to create hash-check for each entry.
     * @param expectedNumEntries The expected amount of entries to be placed into the IBLT
     * @param valueSize The size of the values being added, in bits
     */
    IBLT(long numHashes, long numHashCheck, size_t expectedNumEntries, size_t valueSize);


    bool listEntries(vector<pair<ZZ, ZZ>> &positive,
                     vector<pair<ZZ, ZZ>> &negative,
                     vector<ZZ> &OMSKeys,
                     vector<ZZ> &SMOKeys);

    bool partialPeelFromCells(
            const std::vector<size_t>& cellIndices,
            std::vector<std::pair<NTL::ZZ, NTL::ZZ>>& decodedEntries);

};

#endif //GENSYNCLIB_IBLT_H