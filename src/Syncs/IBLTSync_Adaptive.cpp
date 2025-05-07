//
// Created by Xingyu Chen on 4/27/25.
//

#include <GenSync/Syncs/IBLTSync_Adaptive.h>
#include <GenSync/Aux/Exceptions.h>

IBLTSync_Adaptive::IBLTSync_Adaptive(size_t initExpected, size_t eltSize) {
    initExpNumElems = initExpected;
    elementSize = eltSize;

//    myIBLT = IBLT::Builder().
//            setNumHashes(4).
//            setNumHashCheck(11).
//            setExpectedNumEntries(initExpNumElems).
//            setValueSize(elementSize).
//            build();
}

IBLTSync_Adaptive::~IBLTSync_Adaptive() = default;

bool IBLTSync_Adaptive::SyncClient(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncClient");

    size_t currentExpected = initExpNumElems;
    std::cout << "Entering SyncClient " << std::endl;

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        IBLT myIBLT = IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();
        std::cout << "[Adaptive IBLT] Attempt with expectedNumEntries = " << currentExpected << std::endl;

        for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++) {
            myIBLT.insert((**iter).to_ZZ(), (**iter).to_ZZ());
        }
        std::cout << "myIBLT.insert Done?" << std::endl;



        mySyncStats.timerStart(SyncStats::COMM_TIME);
        if (!commSync->establishIBLTSend(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncClient.");
        }
        commSync->commSend(myIBLT, true);
        bool success = commSync->commRecv_int();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if (success) {
            mySyncStats.timerStart(SyncStats::COMM_TIME);
            list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
            list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.timerStart(SyncStats::COMP_TIME);
            otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
            selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            return true;
        } else {
            Logger::gLog(Logger::METHOD_DETAILS, "Sync failed. Doubling IBLT size to " + toStr(currentExpected * 2));
            currentExpected *= 2;
            commSync->commClose();
        }
    }
}

bool IBLTSync_Adaptive::SyncServer(const shared_ptr<Communicant>& commSync,
                                   list<shared_ptr<DataObject>> &selfMinusOther,
                                   list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering IBLTSync_Adaptive::SyncServer");
    size_t currentExpected = initExpNumElems;
    std::cout << "Entering Server side.." << std::endl;

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while (true) {
        IBLT myIBLT = IBLT::Builder()
                .setNumHashes(4)
                .setNumHashCheck(11)
                .setExpectedNumEntries(currentExpected)
                .setValueSize(elementSize)
                .build();
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        bool oneWay;
        if (!commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), false)) {
            Logger::error_and_quit("IBLT parameter mismatch during SyncServer.");
        }
        std::cout << "Server: size check passed" << std::endl;
        IBLT clientIBLT = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        std::cout << "Server: clientIBLT received" << std::endl;


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

        if (peelSuccess) {
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            for (const auto& p : positive) {
                otherMinusSelf.push_back(make_shared<DataObject>(p.second));
            }
            for (const auto& p : negative) {
                selfMinusOther.push_back(make_shared<DataObject>(p.first));
            }
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(selfMinusOther);
            commSync->commSend(otherMinusSelf);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            return true;
        }
    }
}

//bool IBLTSync_Adaptive::SyncServer(const std::shared_ptr<Communicant>& commSync,
//                                   std::list<std::shared_ptr<DataObject>>& selfMinusOther,
//                                   std::list<std::shared_ptr<DataObject>>& otherMinusSelf) {
//    mySyncStats.timerStart(SyncStats::IDLE_TIME);
//    commSync->commListen();
//    mySyncStats.timerEnd(SyncStats::IDLE_TIME);
//
//    while (true) {
//        std::cout << "[Adaptive IBLT] Server attempting sync with expectedNumEntries = "
//                  << currentExpectedEntries << std::endl;
//
//        mySyncStats.timerStart(SyncStats::COMM_TIME);
//        commSync->establishIBLTRecv(myIBLT.size(), myIBLT.eltSize(), false);
//        IBLT theirs = commSync->commRecv_IBLT(myIBLT.size(), myIBLT.eltSize());
//        mySyncStats.timerEnd(SyncStats::COMM_TIME);
//
//        mySyncStats.timerStart(SyncStats::COMP_TIME);
//        std::vector<std::pair<ZZ, ZZ>> positive, negative;
//        bool success = (theirs -= myIBLT).listEntries(positive, negative);
//        mySyncStats.timerEnd(SyncStats::COMP_TIME);
//
//        mySyncStats.timerStart(SyncStats::COMM_TIME);
//        commSync->commSend(success);
//        mySyncStats.timerEnd(SyncStats::COMM_TIME);
//
//        if (success) {
//            mySyncStats.timerStart(SyncStats::COMP_TIME);
//            for (const auto& p : positive) {
//                otherMinusSelf.push_back(std::make_shared<DataObject>(p.second));
//            }
//            for (const auto& p : negative) {
//                selfMinusOther.push_back(std::make_shared<DataObject>(p.first));
//            }
//            mySyncStats.timerEnd(SyncStats::COMP_TIME);
//
//            commSync->commSend(selfMinusOther);
//            commSync->commSend(otherMinusSelf);
//            break;
//        } else {
//            // Sync失败，扩大IBLT
//            currentExpectedEntries *= 2;
//            rebuildIBLT();
//        }
//    }
//
//    return true;
//}

bool IBLTSync_Adaptive::addElem(shared_ptr<DataObject> datum) {
    return SyncMethod::addElem(datum);
}

bool IBLTSync_Adaptive::delElem(shared_ptr<DataObject> datum) {
    return SyncMethod::delElem(datum);
}

string IBLTSync_Adaptive::getName() {
    return "IBLTSync_Adaptive\n   * initial expected elements = " + toStr(initExpNumElems) +
           "\n   * element size = " + toStr(elementSize) + '\n';
}

