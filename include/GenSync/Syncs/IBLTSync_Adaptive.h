/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/**
 * The {@link IBLTSync_Adaptive} method syncs with another {@link IBLTSync_Adaptive} method by sending an IBLT containing
 * its set. Upon receiving this IBLT, the server performs a subtract operation on both IBLTs
 * and uses the resulting IBLT to calculate the symmetric set difference. These differences
 * are then sent back to the client. If the decoding process failed, we construct an IBLT with doubled size and re-run the synchronization
 * There is an adjustable probability that the sync will fail to recover all differences.
 *
 * Created by Xingyu Chen on 4/27/25.
 */

#ifndef GENSYNC_IBLTSYNC_ADAPTIVE_H
#define GENSYNC_IBLTSYNC_ADAPTIVE_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Syncs/IBLT.h>

/**
 * IBLTSync_Adaptive class dynamically grows the IBLT if reconciliation fails.
 */
class IBLTSync_Adaptive : public SyncMethod
{
public:
    /**
     * Constructor.
     * @param initExpected The initial guess of expected number of elements being stored
     * @param eltSize The size of elements being sent over between client and server
     */
    explicit IBLTSync_Adaptive(size_t initExpected, size_t eltSize);

    ~IBLTSync_Adaptive() override;

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>> &selfMinusOther,
                    list<shared_ptr<DataObject>> &otherMinusSelf) override;

    bool SyncServer(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>> &selfMinusOther,
                    list<shared_ptr<DataObject>> &otherMinusSelf) override;

    bool addElem(shared_ptr<DataObject> datum) override;
    bool delElem(shared_ptr<DataObject> datum) override;

    string getName() override;

    /* Getters for the parameters set in the constructor */
    size_t getInitExpNumElems() const {return initExpNumElems;}
    size_t getElementSize() const {return elementSize;}
private:
    // Initial value of estimated number of difference
    size_t initExpNumElems;

    // Size of elements
    size_t elementSize;
};

#endif //GENSYNC_IBLTSYNC_ADAPTIVE_H
