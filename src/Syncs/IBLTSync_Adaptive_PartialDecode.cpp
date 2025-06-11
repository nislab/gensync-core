//
// Created by ChenXingyu on 6/5/25.
//

#include <GenSync/Syncs/IBLT.h>
#include <GenSync/Syncs/Adaptive_IBLT.h>
#include <GenSync/Syncs/IBLTSync_Adaptive_PartialDecode.h>
#include <GenSync/Aux/Exceptions.h>
#include <NTL/ZZ.h>
#include <functional>
#include <unordered_set>

IBLTSync_Adaptive_PartialDecode::IBLTSync_Adaptive_PartialDecode(size_t initExpected, size_t eltSize) {
    initExpNumElems = initExpected;
    elementSize = eltSize;

    myIBLT = IBLT::Builder().
            setNumHashes(4).
            setNumHashCheck(11).
            setExpectedNumEntries(initExpNumElems).
            setValueSize(elementSize).
            build();
}

IBLTSync_Adaptive_PartialDecode::~IBLTSync_Adaptive_PartialDecode() = default;

bool IBLTSync_Adaptive_PartialDecode::SyncClient(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncClient");

    size_t currentExpected = initExpNumElems;
    unordered_set<ZZ> peeledKeys;

    // call parent method for bookkeeping
    SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

    // connect to server
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    // keep running until peeling succeed
    while (true) {
        commSync->commSend(static_cast<int>(currentExpected));

        // construct new IBLT with updated size
        myIBLT = IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();

//        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
//  //            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
//            ZZ key = (**iter).to_ZZ();
//            if (peeledKeys.find(key) == peeledKeys.end()) {
//                myIBLT.insert(key, key);
//            }
//        }
        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            ZZ key = (**iter).to_ZZ();

            if (peeledKeys.find(key) != peeledKeys.end()) {
                std::cout << "[Client] Skip inserting already peeled key: " << key << std::endl;
                continue;
            }
            myIBLT.insert(key, key);
        }

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

//        mySyncStats.timerStart(SyncStats::COMM_TIME);
//        pendingOMS = commSync->commRecv_DataObject_List();
//        pendingSMO = commSync->commRecv_DataObject_List();
//        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        size_t totalDecoded = static_cast<size_t>(commSync->commRecv_int());

//        mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
//        mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());

        if (!success) {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2 - totalDecoded));
            vector<ZZ> newPeeledKeys = commSync->commRecv_vector_ZZ();
            peeledKeys.insert(newPeeledKeys.begin(), newPeeledKeys.end());
//            vector<pair<ZZ, ZZ>> decodedFromCells;
//            bool recoverSuccess = myIBLT.partialPeelFromCells(peeledIndices, decodedFromCells);
//            if (!recoverSuccess) {
//                cout << "[Client] Recover Failed!" << endl;
//            }
//            for (const auto& p : decodedFromCells) {
//                peeledKeys.insert(p.first);
//            }
            // For test
            std::cout << "[Client] Keys peeled from server response: ";
            for (const auto& key : newPeeledKeys) {
                std::cout << key << " ";
            }
            std::cout << std::endl;
            //
            currentExpected = (currentExpected - totalDecoded) * 2;
            cout << "[Client]: Current Size: " << currentExpected << std::endl;
        } else {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
//            for (const auto& obj : newOMS) peeledKeys.insert(obj->to_ZZ());
//            for (const auto& obj : newSMO) peeledKeys.insert(obj->to_ZZ());

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
    unordered_set<ZZ> peeledKeys;

    // call parent method for bookkeeping
    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    // listen for client
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        currentExpected = static_cast<size_t>(commSync->commRecv_int());

        // construct new IBLT with updated size
        myIBLT = Adaptive_IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();

        std::cout << "[Server]: Current Size: " << currentExpected << std::endl;
        mySyncStats.timerStart(SyncStats::COMM_TIME);
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

        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            ZZ key = (**iter).to_ZZ();

            if (peeledKeys.find(key) != peeledKeys.end()) {
                std::cout << "[Server] Skip inserting already peeled key: " << key << std::endl;
                continue;
            }
            myIBLT.insert(key, key);
        }

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        vector<pair<ZZ, ZZ>> positive, negative;
        vector<ZZ> OMSKeys, SMOKeys;
        clientIBLT -= myIBLT;
        bool peelSuccess = clientIBLT.listEntriesandKeys(positive, negative, OMSKeys, SMOKeys);
        peeledKeys.insert(SMOKeys.begin(), SMOKeys.end());
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // For test
//        std::cout << "[Server] Peeled Keys: ";
//        for (ZZ i : peeledKeys) {
//            std::cout << i << " ";
//        }
        //

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

//            for (const auto& key : peeledKeys) {
//                auto dataPtr = make_shared<DataObject>(key);
//                newOMS.push_back(dataPtr);
//            }
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());

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
    myIBLT.insert(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

bool IBLTSync_Adaptive_PartialDecode::delElem(shared_ptr<DataObject> datum) {
    SyncMethod::delElem(datum);
    myIBLT.erase(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

string IBLTSync_Adaptive_PartialDecode::getName() {
    return "IBLTSync_Adaptive\n   * initial expected elements = " + toStr(initExpNumElems) +
           "\n   * element size = " + toStr(elementSize) + '\n';
}