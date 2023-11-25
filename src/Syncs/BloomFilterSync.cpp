#include <GenSync/Aux/Exceptions.h>
#include <GenSync/Syncs/BloomFilterSync.h>

BloomFilterSync::BloomFilterSync(size_t expected, size_t eltSize)
{
	myBloomFilter = BloomFilter(5,3);
	
	expNumElems = expected;
	elementSize = eltSize;
}

BloomFilterSync::~BloomFilterSync() = default;

bool BloomFilterSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
	Logger::gLog(Logger::METHOD, "Entering BloomFilterSync::SyncClient");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf);

        // connect to server
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commConnect();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

	mySyncStats.timerStart(SyncStats::COMM_TIME);
	commSync->commSend(myBloomFilter.toString());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

	// Computation
	mySyncStats.timerStart(SyncStats::COMM_TIME);
        list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
        list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
        selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        stringstream msg;
        msg << "BloomFilterSync succeeded." << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

	// Record Stats
	mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
}

bool BloomFilterSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
        Logger::gLog(Logger::METHOD, "Entering BloomFilterSync::SyncServer");

        bool success = true;

        // call parent method for bookkeeping
        SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf);

        // listen for client
        mySyncStats.timerStart(SyncStats::IDLE_TIME);
        commSync->commListen();
        mySyncStats.timerEnd(SyncStats::IDLE_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        // communication of bloom filters
        string theirBF = commSync->commRecv_string();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        // Implementation of sync algorithm
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(selfMinusOther);
        commSync->commSend(otherMinusSelf);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        stringstream msg;
        msg << "BloomFilterSync " << (success ? "succeeded" : "may not have completely succeeded") << endl;
        msg << "self - other = " << printListOfSharedPtrs(selfMinusOther) << endl;
        msg << "other - self = " << printListOfSharedPtrs(otherMinusSelf) << endl;
        Logger::gLog(Logger::METHOD, msg.str());

	//Record Stats
        mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
        mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());

        return success;
}

bool BloomFilterSync::addElem(shared_ptr<DataObject> datum)
{
	SyncMethod::addElem(datum);
	myBloomFilter.insert(datum->to_ZZ());
	return true;
}

bool BloomFilterSync::delElem(shared_ptr<DataObject> datum)
{
	SyncMethod::delElem(datum);
	// cannot erase from bloom filter
	return true;
}

string BloomFilterSync::getName()
{
	return "BloomFilterSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(elementSize) + '\n';
}
