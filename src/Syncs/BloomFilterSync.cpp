/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 12/4/2023.
//

#include <GenSync/Aux/Exceptions.h>
#include <GenSync/Syncs/BloomFilterSync.h>
#include <iostream>

BloomFilterSync::BloomFilterSync(size_t expected, size_t eltSize, int szMult, int nHash)
{
	expNumElems = expected;
	elementSize = eltSize;
        sizeMultiplier = szMult;
        numHashes = nHash;
	myBloomFilter = BloomFilter(expNumElems*sizeMultiplier, numHashes);
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

        // Verify server and client Bloom Filters have matching size and numHashes otherwise fail
        mySyncStats.timerStart(SyncStats::COMM_TIME);

        int clientBFsize = myBloomFilter.getSize();
        int clientNumHashes = myBloomFilter.getNumHashes();

        int serverBFsize = commSync->commRecv_int();
        int serverNumHashes = commSync->commRecv_int();

        commSync->commSend(clientBFsize);
        commSync->commSend(clientNumHashes);

        if((clientBFsize != serverBFsize) || (clientNumHashes != serverNumHashes)) {
            Logger::gLog(Logger::METHOD_DETAILS, "BloomFilter parameters do not match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        // send client's bloomFilter to server
	mySyncStats.timerStart(SyncStats::COMM_TIME);
	commSync->commSend(myBloomFilter.toZZ(myBloomFilter.toString()));
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

	// Recieve OMS list from server's computation
	mySyncStats.timerStart(SyncStats::COMM_TIME);
        list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // receive server's bloomFilter
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        ZZ theirBFZZ = commSync->commRecv_ZZ();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        string theirBF = myBloomFilter.ZZtoBitString(theirBFZZ);

        // Determine SMO list from server's bloom filter
        mySyncStats.timerStart(SyncStats::COMP_TIME);
	for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
	{
		if(!myBloomFilter.exist((**iter).to_ZZ(), theirBF))
                {
                        selfMinusOther.push_back(make_shared<DataObject>(**iter));
                }
	}
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // Send SMO list to server
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(selfMinusOther);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

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

        // Verify server and client Bloom Filters have matching size and numHashes otherwise fail
        mySyncStats.timerStart(SyncStats::COMM_TIME);

        int serverBFsize = myBloomFilter.getSize();
        int serverNumHashes = myBloomFilter.getNumHashes();
        
        commSync->commSend(serverBFsize);
        commSync->commSend(serverNumHashes);

        int clientBFsize = commSync->commRecv_int();
        int clientNumHashes = commSync->commRecv_int();

        if((clientBFsize != serverBFsize) || (clientNumHashes != serverNumHashes)) {
            Logger::gLog(Logger::METHOD_DETAILS, "BloomFilter parameters do not match up between client and server!");
            mySyncStats.timerEnd(SyncStats::COMM_TIME);
            mySyncStats.increment(SyncStats::XMIT,commSync->getXmitBytes());
            mySyncStats.increment(SyncStats::RECV,commSync->getRecvBytes());
            return false;
        }
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

	// receive client's bloomFilter
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        ZZ theirBFZZ = commSync->commRecv_ZZ();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        string theirBF = myBloomFilter.ZZtoBitString(theirBFZZ);

        // Determine SMO list from client's bloom filter
        mySyncStats.timerStart(SyncStats::COMP_TIME);
	for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
	{
		if(!myBloomFilter.exist((**iter).to_ZZ(), theirBF))
                {
                        selfMinusOther.push_back(make_shared<DataObject>(**iter));
                }
	}
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

        // Send SMO list to client
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(selfMinusOther);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        // Send server's bloomFilter to client
        mySyncStats.timerStart(SyncStats::COMM_TIME);
	commSync->commSend(myBloomFilter.toZZ(myBloomFilter.toString()));
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        // Recieve OMS list from client's computation
	mySyncStats.timerStart(SyncStats::COMM_TIME);
        list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
        otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
        mySyncStats.timerEnd(SyncStats::COMP_TIME);

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

string BloomFilterSync::getName()
{
	return "BloomFilterSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(elementSize) + '\n';
}
