//
// Created by ChenXingyu on 6/14/25.
//

#include <GenSync/Syncs/RIBLTSync.h>
#include <GenSync/Aux/Logger.h>
#include <iostream>
#include <unordered_set>

using namespace std;

RIBLTSync::RIBLTSync(size_t eltSize) {
    elementSize = eltSize;
}

RIBLTSync::~RIBLTSync() = default;

bool RIBLTSync::SyncClient(const shared_ptr<Communicant>& commSync,
                           list<shared_ptr<DataObject>> &selfMinusOther,
                           list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering RIBLTSync::SyncClient");

    SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

    // connect to server
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    mySyncStats.timerStart(SyncStats::COMP_TIME);
    RIBLT riblt_client(elementSize);
    auto& encoder = riblt_client.getEncoder();

    for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); ++iter) {
        ZZ key = (**iter).to_ZZ();
        auto obj = make_shared<DataObject>(key);
        auto sym = make_shared<DataObjectSymbolWrapper>(obj);
        encoder.addSymbol(sym);
    }

    mySyncStats.timerEnd(SyncStats::COMP_TIME);

    std::thread listenerThread(&RIBLTSync::listenForDone, this, commSync, std::ref(serverDone));

    while (!serverDone) {
        try {
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            auto symbol = encoder.produceNextCell();
            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(symbol);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
        }
        catch (Communicant::ConnectionClosedException& e) {
            // Handle reconnection or abort if needed
            break;
        }
    }

    // Wait for thread to join if not done already
    if (listenerThread.joinable()) listenerThread.join();

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

void RIBLTSync::listenForDone(const std::shared_ptr<Communicant>& commSync, std::atomic<bool>& doneFlag) {
    while (!doneFlag) {
        try {
            auto msg = commSync->commRecv(1);
            if (!msg.empty() && msg == "d"){
                doneFlag = true;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Avoid busy waiting
        } catch (const Communicant::ConnectionClosedException& e) {
            Logger::gLog(Logger::COMM_DETAILS, "Connection closed in background recv thread");
            break;
        }
    }
}


bool RIBLTSync::SyncServer(const shared_ptr<Communicant>& commSync,
                           list<shared_ptr<DataObject>> &selfMinusOther,
                           list<shared_ptr<DataObject>> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD, "Entering RIBLTSync::SyncServer");

    // call parent method for bookkeeping
    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

    // listen for client
    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    mySyncStats.timerStart(SyncStats::COMP_TIME);
    RIBLT riblt_server(elementSize);
    auto& decoder = riblt_server.getDecoder();

    for (auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); ++iter) {
        ZZ key = (**iter).to_ZZ();
        auto obj = make_shared<DataObject>(key);
        auto sym = make_shared<DataObjectSymbolWrapper>(obj);
        decoder.addSymbol(sym);
    }
    mySyncStats.timerEnd(SyncStats::COMP_TIME);

    list<shared_ptr<DataObject>> OMS, SMO;

    while (true) {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        auto symbol = commSync->commRecv_CodedSymbol();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        decoder.addCodedSymbol(symbol);
        bool peelSuccess = decoder.tryDecode();
        mySyncStats.timerEnd(SyncStats::COMP_TIME);
        
        if (peelSuccess) {
            commSync->commSend("d", 1);
            mySyncStats.timerStart(SyncStats::COMP_TIME);
            auto OMS = decoder.getOMS();
            auto SMO = decoder.getSMO();

            for (const ZZ& val : OMS) {
                otherMinusSelf.push_back(make_shared<DataObject>(val));
            }
            for (const ZZ& val : SMO) {
                selfMinusOther.push_back(make_shared<DataObject>(val));
            }

            mySyncStats.timerEnd(SyncStats::COMP_TIME);

            mySyncStats.timerStart(SyncStats::COMM_TIME);
            commSync->commSend(selfMinusOther);
            commSync->commSend(otherMinusSelf);
            mySyncStats.timerEnd(SyncStats::COMM_TIME);

            mySyncStats.increment(SyncStats::XMIT, commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV, commSync->getRecvBytes());
            return true;
        } else 
        {
        }
    }
}

bool RIBLTSync::addElem(shared_ptr<DataObject> datum)
{
    // call parent add
    SyncMethod::addElem(datum);
    return true;
}

bool RIBLTSync::delElem(shared_ptr<DataObject> datum)
{
    // call parent delete
    SyncMethod::delElem(datum);
//    myRIBLT->erase(datum->to_ZZ());
    return true;
}

string RIBLTSync::getName()
{
    return "Rateless IBLT Sync:   Size of values = " + toStr(elementSize) + "\n";
}

