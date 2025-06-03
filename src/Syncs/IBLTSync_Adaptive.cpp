//
// Created by Xingyu Chen on 4/27/25.
//

#include <GenSync/Syncs/IBLTSync_Adaptive.h>
#include <GenSync/Aux/Exceptions.h>

IBLTSync_Adaptive::IBLTSync_Adaptive(size_t initExpected, size_t eltSize) {
    initExpNumElems = initExpected;
    elementSize = eltSize;

    myIBLT = IBLT::Builder().
            setNumHashes(4).
            setNumHashCheck(11).
            setExpectedNumEntries(initExpNumElems).
            setValueSize(elementSize).
            build();
}

IBLTSync_Adaptive::~IBLTSync_Adaptive() = default;

bool IBLTSync_Adaptive::SyncClient(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncClient");

    size_t currentExpected = initExpNumElems;
    list<shared_ptr<DataObject>> pendingOMS, pendingSMO;

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

        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
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

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        pendingOMS = commSync->commRecv_DataObject_List();
        pendingSMO = commSync->commRecv_DataObject_List();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        size_t totalDecoded = pendingOMS.size() + pendingSMO.size();

        if (!success) {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2 - totalDecoded));
            currentExpected = currentExpected * 2 - totalDecoded;
            cout << "[Client]: Current Size: " << currentExpected << std::endl;
        } else {
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            otherMinusSelf.insert(otherMinusSelf.end(), pendingOMS.begin(), pendingOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), pendingSMO.begin(), pendingSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);
            return true;
        }

        mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
    }
}
        // insert the decoded elements and record Stats
//        if (success) {
//            mySyncStats.timerStart(SyncStats::COMM_TIME);
//            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
//            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
//            mySyncStats.timerEnd(SyncStats::COMM_TIME);
//
//            mySyncStats.timerStart(SyncStats::COMP_TIME);
//            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
//            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
//            mySyncStats.timerEnd(SyncStats::COMP_TIME);
//
//            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
//            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
//
//            return true;
//        } else {
//            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Doubling IBLT size to " + toStr(currentExpected * 2));
//            currentExpected *= 2;
// //            commSync->commClose();
//        }

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

    while (true) {
        currentExpected = static_cast<size_t>(commSync->commRecv_int());

        // construct new IBLT with updated size
        myIBLT = IBLT::Builder()
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
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        vector<pair<ZZ, ZZ>> positive, negative;
        bool peelSuccess = (clientIBLT -= myIBLT).listEntries(positive, negative);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(peelSuccess);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);


        mySyncStats.timerStart(SyncStats::COMP_TIME);
        list<shared_ptr<DataObject>> tempOMS, tempSMO;
        for (const auto& p : positive) {
            tempOMS.push_back(make_shared<DataObject>(p.second));
        }
        for (const auto& p : negative) {
            tempSMO.push_back(make_shared<DataObject>(p.first));
        }
        size_t totalDecoded = tempOMS.size() + tempSMO.size();
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(tempSMO);
        commSync->commSend(tempOMS);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());

        if (peelSuccess) {
            selfMinusOther.insert(selfMinusOther.end(), tempSMO.begin(), tempSMO.end());
            otherMinusSelf.insert(otherMinusSelf.end(), tempOMS.begin(), tempOMS.end());
            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Adjusting IBLT size to " + toStr(currentExpected * 2 - totalDecoded));
        }
//        // store decoded elements and record Stats
//        if (peelSuccess) {
//            mySyncStats.timerStart(SyncStats::COMP_TIME);
//            for (const auto& p : positive) {
//                otherMinusSelf.push_back(make_shared<DataObject>(p.second));
//            }
//            for (const auto& p : negative) {
//                selfMinusOther.push_back(make_shared<DataObject>(p.first));
//            }
//            mySyncStats.timerEnd(SyncStats::COMP_TIME);
//
//            mySyncStats.timerStart(SyncStats::COMM_TIME);
//            commSync->commSend(selfMinusOther);
//            commSync->commSend(otherMinusSelf);
//            mySyncStats.timerEnd(SyncStats::COMM_TIME);
//
//            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
//            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
//
//            return true;
//        } else {
//            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Doubling IBLT size to " + toStr(currentExpected * 2));
//            currentExpected *= 2;
////            commSync->commClose();
//        }
    }
}

bool IBLTSync_Adaptive::addElem(shared_ptr<DataObject> datum) {
    SyncMethod::addElem(datum);
    myIBLT.insert(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

bool IBLTSync_Adaptive::delElem(shared_ptr<DataObject> datum) {
    SyncMethod::delElem(datum);
    myIBLT.erase(datum->to_ZZ(), datum->to_ZZ());
    return true;
}

string IBLTSync_Adaptive::getName() {
    return "IBLTSync_Adaptive\n   * initial expected elements = " + toStr(initExpNumElems) +
           "\n   * element size = " + toStr(elementSize) + '\n';
}

