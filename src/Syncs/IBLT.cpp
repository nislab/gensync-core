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
                       vector<ZZ> &OMSKeys,
                       vector<ZZ> &SMOKeys) {
    OMSKeys.clear();
    SMOKeys.clear();
    long nErased;
    do {
        nErased = 0;
        for (size_t i = 0; i < hashTable.size(); ++i) {
            auto& entry = hashTable[i];
            if (entry.isPure(numHashCheck)) {
                if (entry.count == 1) {
                    positive.emplace_back(entry.keySum, entry.valueSum);
                    OMSKeys.push_back(entry.keySum);  // record key of positive
                } else {
                    negative.emplace_back(entry.keySum, entry.valueSum);
                    SMOKeys.push_back(entry.keySum);  // record key of negative
                }
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                ++nErased;
            }
        }
    } while (nErased > 0);

    for (const auto& entry : hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}


bool IBLT::partialPeelFromCells(
        const std::vector<size_t>& cellIndices,
        std::vector<std::pair<NTL::ZZ, NTL::ZZ>>& decodedEntries)
{
    long nErased;
    unordered_set<size_t> activeCells(cellIndices.begin(), cellIndices.end());
    unordered_set<size_t> peeledSet;

    do {
        nErased = 0;
        unordered_set<size_t> newActiveCells;

        for (size_t i : activeCells) {
            if (i >= hashTable.size() || peeledSet.count(i)) continue;
            auto& entry = hashTable[i];

            if (entry.isPure(numHashCheck)) {
                decodedEntries.emplace_back(entry.keySum, entry.valueSum);
                peeledSet.insert(i);

                long numHashesToUse = (calcNumHashes != nullptr) ? calcNumHashes(entry.keySum) : numHashes;
                long bucketsPerHash = hashTable.size() / numHashesToUse;

                for (int h = 0; h < numHashesToUse; ++h) {
                    hash_t hk = _hashK(entry.keySum, h);
                    size_t target = h * bucketsPerHash + (hk % bucketsPerHash);
                    if (target < hashTable.size())
                        newActiveCells.insert(target);
                }

                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                ++nErased;
            }
        }

        activeCells = std::move(newActiveCells);

    } while (nErased > 0);

    for (const auto& entry : hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}

