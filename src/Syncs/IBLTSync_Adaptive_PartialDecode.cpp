//
// Created by ChenXingyu on 6/5/25.
//

#include <GenSync/Syncs/IBLT.h>
#include <GenSync/Syncs/IBLTSync_Adaptive_PartialDecode.h>
#include <GenSync/Aux/Exceptions.h>
#include <NTL/ZZ.h>
#include <functional>
#include <unordered_set>

IBLTSync_Adaptive_PartialDecode::IBLTSync_Adaptive_PartialDecode(size_t initExpected, size_t eltSize) {
    initExpNumElems = initExpected;
    elementSize = eltSize;
}

IBLTSync_Adaptive_PartialDecode::~IBLTSync_Adaptive_PartialDecode() = default;

bool IBLTSync_Adaptive_PartialDecode::SyncClient(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncClient");

    size_t currentExpected = initExpNumElems;
    unordered_set<ZZ, HashZZ> peeledKeys;

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
            ZZ key = (**iter).to_ZZ();
            if (peeledKeys.find(key) != peeledKeys.end()) {
                continue;
            }
            myIBLT.insert(key, key);
        }
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // ensure that the IBLT size and eltSize equal those of the server otherwise fail and don't continue
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncClient.");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        commSync->commSend(myIBLT, true);
        bool success = commSync->commRecv_int();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        size_t totalDecoded = static_cast<size_t>(commSync->commRecv_int());


        if (!success) {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2 - totalDecoded));

            mySyncStats.timerStart(SyncStats::COMM_TIME);
            vec_ZZ newPeeledKeys = commSync->commRecv_vec_ZZ();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            for (long i = 0; i < newPeeledKeys.length(); ++i) {
                peeledKeys.insert(newPeeledKeys[i]);
            }

            currentExpected = (currentExpected - totalDecoded) * 2;
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

        } else {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            for (const auto& key : peeledKeys) {
                auto dataPtr = make_shared<DataObject>(key);
                newOMS.push_back(dataPtr);
            }

            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
            return true;
        }
    }
}

bool IBLTSync_Adaptive_PartialDecode::SyncServer(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncServer");
    size_t currentExpected = initExpNumElems;
    unordered_set<ZZ, HashZZ> peeledKeys;

    // call parent method for bookkeeping
    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    // listen for client
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        currentExpected = static_cast<size_t>(commSync->commRecv_int());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // construct new IBLT with updated size
        IBLT myIBLT = IBLT::Builder()
                .setNumHashes(DEFAULT_NUM_HASHES)
                .setNumHashCheck(DEFAULT_NUM_HASH_CHECK)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();

        // ensure that the IBLT size and eltSize equal those of the client otherwise fail and don't continue
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncServer.");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }

        // verified that our size and eltSize == theirs
        IBLT clientIBLT = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            ZZ key = (**iter).to_ZZ();

            if (peeledKeys.find(key) != peeledKeys.end()) {
                std::cout << "[Server] Skip inserting already peeled key: " << key << std::endl;
                continue;
            }
            myIBLT.insert(key, key);
        }

        vector<pair<ZZ, ZZ>> positive, negative;
        vec_ZZ OMSKeys, SMOKeys;
        clientIBLT -= myIBLT;
        bool peelSuccess = clientIBLT.listEntriesandKeys(positive, negative, OMSKeys, SMOKeys);
        for (long i = 0; i < SMOKeys.length(); ++i) {
            peeledKeys.insert(SMOKeys[i]);
        }
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(peelSuccess);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);


        mySyncStats.timerStart(SyncStats::COMP_TIME);
        list<shared_ptr<DataObject>> newOMS, newSMO;
        for (const auto& p : positive) {
            newOMS.push_back(make_shared<DataObject>(p.second));
        }
        for (const auto& p : negative) {
            newSMO.push_back(make_shared<DataObject>(p.first));
        }

        size_t totalDecoded = newOMS.size() + newSMO.size();
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(static_cast<int>(totalDecoded));
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (peelSuccess) {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(newSMO);
            commSync->commSend(newOMS);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2 - totalDecoded));
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(SMOKeys);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
        }
    }
}

bool IBLTSync_Adaptive_PartialDecode::addElem(shared_ptr<DataObject> datum) {
    SyncMethod::addElem(datum);
    return true;
}

bool IBLTSync_Adaptive_PartialDecode::delElem(shared_ptr<DataObject> datum) {
    SyncMethod::delElem(datum);
    return true;
}

string IBLTSync_Adaptive_PartialDecode::getName() {
    return "IBLTSync_Adaptive\n   * initial expected elements = " + toStr(initExpNumElems) +
           "\n   * element size = " + toStr(elementSize) + '\n';
}