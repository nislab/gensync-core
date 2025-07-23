//
// Created by ChenXingyu on 6/10/25.
//

#ifndef GENSYNC_ADAPTIVE_IBLT_H
#define GENSYNC_ADAPTIVE_IBLT_H

#include <GenSync/Syncs/IBLT.h>
#include <unordered_set>

namespace std {
    template <>
    struct hash<NTL::ZZ> {
        std::size_t operator()(const NTL::ZZ& z) const {
            return std::hash<std::string>()(zzToString(z));
        }
    };
}

class Adaptive_IBLT : public IBLT {
public:
    Adaptive_IBLT(long numHashes, long numHashCheck, size_t expectedNumEntries, size_t valueSize);

    // List entries and record the peeled cell indices
    bool listEntries(std::vector<std::pair<NTL::ZZ, NTL::ZZ>> &positive,
                     std::vector<std::pair<NTL::ZZ, NTL::ZZ>> &negative,
                     std::vector<size_t> &peeledIndicesOut);

    // Return elements reconstructed from cell indices
    std::unordered_set<NTL::ZZ> extractElementsFromIndices(const std::vector<size_t>& indices) const;

private:
    std::vector<size_t> peeledIndices;
};

#endif //GENSYNC_ADAPTIVE_IBLT_H
