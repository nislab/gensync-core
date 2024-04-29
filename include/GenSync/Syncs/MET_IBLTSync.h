/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * Created by Anish Sinha on 3/14/23.
 */
#ifndef GENSYNCLIB_METIBLTSYNC_H
#define GENSYNCLIB_METIBLTSYNC_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Syncs/MET_IBLT.h>

class MET_IBLTSync : public SyncMethod {
public:
    /**
     * Constructor.
     * @param expNumElems The expected number of elements being stored
     * @param eltSize The size of elements being stored
     */
    MET_IBLTSync(size_t expNumElems, size_t eltSize);
    ~MET_IBLTSync() override;

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool addElem(shared_ptr<DataObject> datum) override;
    bool delElem(shared_ptr<DataObject> datum) override;

    string getName() override;

    /* Getters for the parameters set in the constructor */
    size_t getExpNumElems() const {return expNumElems;}
    size_t getElementSize() const {return elementSize;}

protected:
    
private:
    /**
     * MET IBLT instance variable for storing data
     */
    shared_ptr<MET_IBLT> myMET;

    /**
     * Instance variable to store expected number of elements
     */
    size_t expNumElems;

    /**
     * Size of elements as set in the constructor
     */
    size_t elementSize;
};

#endif