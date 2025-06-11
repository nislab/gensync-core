//
// Created by ChenXingyu on 6/5/25.
//

#ifndef GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H
#define GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Syncs/IBLT.h>

/**
 * IBLTSync_Adaptive class dynamically grows the IBLT if reconciliation fails.
 */
class IBLTSync_Adaptive_PartialDecode : public SyncMethod
{
public:
    /*
     * Constructor.
     * @param initExpectedEntries The initial value of expected number of elements being stored
     * @param eltSize The size of elements being stored
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
    // IBLT instance variable for storing data
    IBLT myIBLT;

    // Initial value of estimated number of difference
    size_t initExpNumElems;

    // Size of elements
    size_t elementSize;
};

#endif //GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODE_H
