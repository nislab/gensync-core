//
// Created by ChenXingyu on 6/10/25.
//
#include <GenSync/Syncs/Adaptive_IBLT.h>
#include <GenSync/Aux/Logger.h>

bool Adaptive_IBLT::listEntries(vector<pair<ZZ, ZZ>> &positive,
                               vector<pair<ZZ, ZZ>> &negative,
                               vector<size_t> &peeledIndicesOut) {
    peeledIndices.clear();
    long nErased;
    do {
        nErased = 0;
        for (size_t i = 0; i < hashTable.size(); ++i) {
            auto& entry = hashTable[i];
            if (entry.isPure(numHashCheck)) {
                if (entry.count == 1) {
                    positive.emplace_back(entry.keySum, entry.valueSum);
                } else {
                    negative.emplace_back(entry.keySum, entry.valueSum);
                }
                this->_insert(-entry.count, entry.keySum, entry.valueSum);
                peeledIndices.push_back(i);
                ++nErased;
            }
        }
    } while (nErased > 0);
    peeledIndicesOut = peeledIndices;

    // If any buckets for one of the hash functions is not empty, then we didn't peel them all
    for (const auto& entry : hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}
