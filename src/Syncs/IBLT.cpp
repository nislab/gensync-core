/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Eliezer Pearl on 7/9/18.
// Based on iblt.cpp and iblt.h in https://github.com/mwcote/IBLT-Research, which are based on:
// * Goodrich, Michael T., and Michael Mitzenmacher. "Invertible bloom lookup tables." 2011 49th Annual Allerton Conference on Communication, Control, and Computing (Allerton). IEEE, 2011.
// * Eppstein, D avid, et al. "What's the difference?: efficient set reconciliation without prior context." ACM SIGCOMM Computer Communication Review 41.4 (2011): 218-229.
//

#include <GenSync/Syncs/IBLT.h>
#include <unordered_set>

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

bool IBLT::listEntriesandKeys(vector<pair<ZZ, ZZ>> &positive,
                              vector<pair<ZZ, ZZ>> &negative,
                              vec_ZZ &OMSKeys,
                              vec_ZZ &SMOKeys) {
    OMSKeys.SetLength(0);
    SMOKeys.SetLength(0);

    long nErased;
    do {
        nErased = 0;
        for (size_t i = 0; i < hashTable.size(); ++i) {
            auto& entry = hashTable[i];
            if (entry.isPure(numHashCheck)) {
                if (entry.count == 1) {
                    positive.emplace_back(entry.keySum, entry.valueSum);
                    append(OMSKeys, entry.keySum);  // record key of positive
                } else {
                    negative.emplace_back(entry.keySum, entry.valueSum);
                    append(SMOKeys, entry.keySum);  // record key of negative
                }
                // Once processed the pure cell, remove it
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                ++nErased;
            }
        }
    } while (nErased > 0);

    // If any buckets for one of the hash functions is not empty, then we didn't peel them all
    for (const auto& entry : hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}