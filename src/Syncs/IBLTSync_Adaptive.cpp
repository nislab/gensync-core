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
    std::cout << "[Client]: Entering SyncClient" << std::endl;

    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        myIBLT = IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();
        std::cout << "[Client]: Attempt with expectedNumEntries = " << currentExpected << std::endl;
        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }
        std::cout << "[Client]: Insert Done" << std::endl;



        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncClient.");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        commSync->commSend(myIBLT, true);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        bool success = commSync->commRecv_int();


        if (success) {
            //mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            //mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());

            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Doubling IBLT size to " + toStr(currentExpected * 2));
            currentExpected *= 2;
//            commSync->commClose();
        }
    }
}

bool IBLTSync_Adaptive::SyncServer(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncServer");
    size_t currentExpected = initExpNumElems;
    std::cout << "[Server]: Entering SyncServer." << std::endl;

    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        myIBLT = IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();
        std::cout << "[Server]: Attempt with expectedNumEntries = " << currentExpected << std::endl;
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncServer.");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        IBLT clientIBLT = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);


        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        vector<pair<ZZ, ZZ>> positive, negative;
        bool peelSuccess = (clientIBLT -= myIBLT).listEntries(positive, negative);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        //mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(peelSuccess);
        //mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (peelSuccess) {
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            for (const auto& p : positive) {
                otherMinusSelf.push_back(make_shared<DataObject>(p.second));
            }
            for (const auto& p : negative) {
                selfMinusOther.push_back(make_shared<DataObject>(p.first));
            }
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            //mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(selfMinusOther);
            commSync->commSend(otherMinusSelf);
            //mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());

            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Doubling IBLT size to " + toStr(currentExpected * 2));
            currentExpected *= 2;
//            commSync->commClose();
        }
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

