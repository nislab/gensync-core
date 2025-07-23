//
// Created by ChenXingyu on 6/10/25.
//
#include <GenSync/Syncs/Adaptive_IBLT.h>
#include <GenSync/Aux/Logger.h>

Adaptive_IBLT::Adaptive_IBLT(long numHashes, long numHashCheck, size_t expectedNumEntries, size_t valueSize)
        : IBLT(numHashes, numHashCheck, expectedNumEntries, valueSize) {}

bool Adaptive_IBLT::listEntries(std::vector<std::pair<NTL::ZZ, NTL::ZZ>> &positive,
                               std::vector<std::pair<NTL::ZZ, NTL::ZZ>> &negative,
                               std::vector<size_t> &peeledIndicesOut) {
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

    for (const auto& entry : hashTable) {
        if (!entry.empty()) return false;
    }
    return true;
}

std::unordered_set<NTL::ZZ> Adaptive_IBLT::extractElementsFromIndices(const std::vector<size_t>& indices) const {
    std::unordered_set<NTL::ZZ> result;
    for (size_t idx : indices) {
        if (idx >= hashTable.size()) continue;
        const auto& entry = hashTable[idx];
        if (entry.isPure(numHashCheck)) {
            result.insert(entry.keySum);
        }
    }
    return result;
}