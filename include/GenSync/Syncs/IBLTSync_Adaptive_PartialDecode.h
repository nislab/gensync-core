/**
 * The {@link IBLTSync_Adaptive_PartialDecode} method syncs with another {@link IBLTSync_Adaptive_PartialDecode} method by sending an IBLT containing
 * its set. Upon receiving this IBLT, the server performs a subtract operation on both IBLTs
 * and uses the resulting IBLT to calculate the symmetric set difference. These differences
 * are then sent back to the client. If the decoding process failed, we construct an new IBLT.
 * The new IBLT is built with doubled size minus the number of peeled elements and we do not insert peeled elements.
 * There is an adjustable probability that the sync will fail to recover all differences.
 * This type of syncs is a variance of {@link IBLTSync_Adaptive}.
 *
 * Created by Xingyu Chen on 6/5/25.
 */

#ifndef GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H
#define GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Syncs/IBLT.h>

/**
 * IBLTSync_Adaptive_PartialDecode class dynamically grows the IBLT if reconciliation fails, it records the indices of peeled elements
 * and do not insert them into the following generated IBLTs.
 */
class IBLTSync_Adaptive_PartialDecode : public SyncMethod
{
public:
    /**
     * Constructor.
     * @param initExpected The initial guess of expected number of elements being stored
     * @param eltSize The size of elements being sent over between client and server
     */
    explicit IBLTSync_Adaptive_PartialDecode(size_t initExpected, size_t eltSize);

    ~IBLTSync_Adaptive_PartialDecode() override;

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

    // Provides a hash function for NTL::ZZ by converting the value to a string and hashing that.
    struct HashZZ {
        size_t operator()(const ZZ& z) const {
            return hash<string>()(zzToString(z));
        }
    };
};

#endif //GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H
