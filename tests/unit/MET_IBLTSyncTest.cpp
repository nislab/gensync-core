/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 4/11/24.
//

#include "MET_IBLTSyncTest.h"
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/MET_IBLTSync.h>
#include "../TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(MET_IBLTSyncTest);

MET_IBLTSyncTest::MET_IBLTSyncTest() = default;

MET_IBLTSyncTest::~MET_IBLTSyncTest() = default;

void MET_IBLTSyncTest::setUp() {
	const int SEED = 93;
	srand(SEED);

	ZZ NTL_SEED = ZZ(93);
    SetSeed(NTL_SEED);
}

void MET_IBLTSyncTest::tearDown() {
}

void MET_IBLTSyncTest::MET_IBLTSyncSetReconcileTest() {
	const int BITS = sizeof(randZZ());
	
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			build();
	
	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			build();
	
	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, false));
}

void MET_IBLTSyncTest::MET_IBLTSyncLargeSetReconcileTest(){
	const int BITS = sizeof(randZZ());
	
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			build();
	
	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			build();
	
	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true,false,false,true));
}

void MET_IBLTSyncTest::testAddElem(){
	// number of elems to add
	const int ITEMS = 50;
	GenSync metSync = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(4).
			build();
	multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;
	
	// add items works
	for(int ii = 0; ii < ITEMS; ii++) {
		shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
		elts.insert(item);
		metSync.addElem(item);
	}
	
	// check that all items added
	CPPUNIT_ASSERT(metSync.dumpElements().size() == ITEMS);
}

void MET_IBLTSyncTest::testGetStrings(){
	GenSync metSync = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::MET_IBLTSync).
			setComm(GenSync::SyncComm::socket).
			setBits(4).
			build();
	
	CPPUNIT_ASSERT(!metSync.getName().empty());
}