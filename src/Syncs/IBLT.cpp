/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18.
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research, which are based on:
// * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 2011 49th Annual Allerton Conference on Communication, Control, and Computing (Allerton). IEEE, 2011.
// * Eppstein, D avid, et al. "What's the difference?: efficient set reconciliation without prior context." ACM SIGCOMM Computer Communication Review 41.4 (2011): 218-229.
//

#include <GenSync/Syncs/IBLT.h>

IBLT::IBLT() = default;
IBLT::~IBLT() = default;

IBLT::IBLT(long numHashes, long numHashCheck, size_t expectedNumEntries, size_t valueSize)
{
    this->numHashes = numHashes;
    this->numHashCheck = numHashCheck;
    this->valueSize = valueSize;

    // 1.5x expectedNumEntries gives very low probability of decoding failure
    size_t nEntries = expectedNumEntries + expectedNumEntries/2;

    // resize cells to be divisible by number of hash
    hashTable.resize(nEntries + nEntries % numHashes);
}
