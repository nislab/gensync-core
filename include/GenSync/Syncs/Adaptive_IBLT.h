//
// Created by ChenXingyu on 6/10/25.
//

#ifndef GENSYNC_ADAPTIVE_IBLT_H
#define GENSYNC_ADAPTIVE_IBLT_H

#include <GenSync/Syncs/IBLT.h>
#include <unordered_set>

class Adaptive_IBLT : public IBLT {

public:

private:
    /**
     * Produces a list of all the key-value pairs and indices of peeled elements in the IBLT.
     * With a low, constant probability, only partial lists will be produced
     * Listing is destructive, as the same peeling technique used in the get method is used.
     * Will remove all key-value pairs from the IBLT that are listed.
     * @param positive All the elements that could be inserted.
     * @param negative All the elements that were removed without being inserted first.
     * @param peeledIndicesOut List of indices of the elements that were successfully peeled out.
     * @return true iff the operation has successfully recovered the entire list
     */
    bool listEntries(vector<pair<ZZ, ZZ>> &positive,
                     vector<pair<ZZ, ZZ>> &negative,
                     vector<size_t> &peeledIndicesOut);

//    // Return elements reconstructed from cell indices
//    unordered_set<ZZ> extractElementsFromIndices(const vector<size_t>& indices) const;

    // List of indices of peeled elements
    vector<size_t> peeledIndices;
};

#endif //GENSYNC_ADAPTIVE_IBLT_H
