//
// Created by ChenXingyu on 6/14/25.
//

#ifndef GENSYNC_RIBLTSYNC_H
#define GENSYNC_RIBLTSYNC_H


#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Syncs/RIBLT.h>
#include <atomic>
#include <thread>

class RIBLTSync : public SyncMethod {
  public:
    /**
     * Constructor.
     * @param eltSize The size of elements being stored
     */
    RIBLTSync(size_t eltSize);
    ~RIBLTSync() override;

    // Implemented parent class methods
    bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
    bool addElem(shared_ptr<DataObject> datum) override;
    bool delElem(shared_ptr<DataObject> datum) override;

    string getName() override;

    /* Getters for the parameters set in the constructor */
    size_t getElementSize() const {return elementSize;}

    std::atomic<bool> serverDone{false};

    void listenForDone(const std::shared_ptr<Communicant>& commSync, std::atomic<bool>& doneFlag);

  protected:

  private:

    /**
     * Size of elements as set in the constructor
     */
    size_t elementSize;
};

#endif // GENSYNC_RIBLTSYNC_H
