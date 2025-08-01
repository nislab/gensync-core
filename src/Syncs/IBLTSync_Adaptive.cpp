//
// Created by Xingyu Chen on 4/27/25.
//

#include <GenSync/Syncs/IBLTSync_Adaptive.h>
#include <GenSync/Aux/Exceptions.h>

IBLTSync_Adaptive::IBLTSync_Adaptive(size_t initExpected, size_t eltSize) {
    initExpNumElems = initExpected;
    elementSize = eltSize;
}

IBLTSync_Adaptive::~IBLTSync_Adaptive() = default;

bool IBLTSync_Adaptive::SyncClient(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncClient");

    size_t currentExpected = initExpNumElems;

    // call parent method for bookkeeping
    SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

    // connect to server
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    // keep running until peeling succeed
    while (true) {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(static_cast<int>(currentExpected));
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        // construct new IBLT with updated size
        IBLT myIBLT = IBLT::Builder()
                .setNumHashes(DEFAULT_NUM_HASHES)
                .setNumHashCheck(DEFAULT_NUM_HASH_CHECK)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();

        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(myIBLT, true);
        bool success = commSync->commRecv_int();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (!success) {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2));
            currentExpected = currentExpected * 2;
        } else {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
            return true;
        }
    }
}

bool IBLTSync_Adaptive::SyncServer(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncServer");
    size_t currentExpected = initExpNumElems;

    // call parent method for bookkeeping
    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    // listen for client
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    // keep generating new IBLT until peeling succeed
    while (true) {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        currentExpected = static_cast<size_t>(commSync->commRecv_int());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        // construct new IBLT with updated size
        mySyncStats.timerStart(SyncStats::COMP_TIME);
        IBLT myIBLT = IBLT::Builder()
                .setNumHashes(DEFAULT_NUM_HASHES)
                .setNumHashCheck(DEFAULT_NUM_HASH_CHECK)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        IBLT clientIBLT = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }

        // Attempt to peel the elements
        vector<pair<ZZ, ZZ>> positive, negative;
        bool peelSuccess = (clientIBLT -= myIBLT).listEntries(positive, negative);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(peelSuccess);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (peelSuccess) {
            // Extract peeled elements
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            list<shared_ptr<DataObject>> tempOMS, tempSMO;
            for (const auto& p : positive) {
                tempOMS.push_back(make_shared<DataObject>(p.second));
            }
            for (const auto& p : negative) {
                tempSMO.push_back(make_shared<DataObject>(p.first));
            }
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(tempSMO);
            commSync->commSend(tempOMS);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            selfMinusOther.insert(selfMinusOther.end(), tempSMO.begin(), tempSMO.end());
            otherMinusSelf.insert(otherMinusSelf.end(), tempOMS.begin(), tempOMS.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2));
        }
    }
}

bool IBLTSync_Adaptive::addElem(shared_ptr<DataObject> datum) {
    SyncMethod::addElem(datum);
    return true;
}

bool IBLTSync_Adaptive::delElem(shared_ptr<DataObject> datum) {
    SyncMethod::delElem(datum);
    return true;
}

string IBLTSync_Adaptive::getName() {
    return "IBLTSync_Adaptive\n   * initial expected elements = " + toStr(initExpNumElems) +
           "\n   * element size = " + toStr(elementSize) + '\n';
}

