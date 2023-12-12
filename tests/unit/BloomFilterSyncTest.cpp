/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 12/4/23.
//

#include "BloomFilterSyncTest.h"
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/BloomFilterSync.h>
#include "../TestAuxiliary.h"
CPPUNIT_TEST_SUITE_REGISTRATION(BloomFilterSyncTest);

BloomFilterSyncTest::BloomFilterSyncTest() = default;

BloomFilterSyncTest::~BloomFilterSyncTest() = default;

void BloomFilterSyncTest::setUp() {
	const int SEED = 93;
	srand(SEED);
}

void BloomFilterSyncTest::tearDown() {
}

void BloomFilterSyncTest::BloomFilterSyncSetReconcileTest() {
	const int BITS = sizeof(randZZ());
	
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();
	
	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();
	
	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, false));
}

void BloomFilterSyncTest::BloomFilterSyncLargeSetReconcileTest(){
	const int BITS = sizeof(randZZ());
	
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(largeNumExpElems).
			build();
	
	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(largeNumExpElems).
			build();
	
	//(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = true)
	CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true,false,false,true));
}

void BloomFilterSyncTest::testAddElem(){
	// number of elems to add
	const int ITEMS = 50;
	GenSync bfSync = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(4).
			setExpNumElems(numExpElem).
			build();
	multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;
	
	// add items works
	for(int ii = 0; ii < ITEMS; ii++) {
		shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
		elts.insert(item);
		bfSync.addElem(item);
	}
	
	// check that all items added
	CPPUNIT_ASSERT(bfSync.dumpElements().size() == ITEMS);
}

void BloomFilterSyncTest::testGetStrings(){
	GenSync bfSync = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(4).
			setExpNumElems(numExpElem).
			build();
	
	CPPUNIT_ASSERT(!bfSync.getName().empty());
}

void BloomFilterSyncTest::testBloomFilterParamMismatch(){
	const int BITS = sizeof(randZZ());
	
	GenSync GenSyncServer = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			//Different number of expectedElements to ensure that mismatches cause failure properly
			setExpNumElems(numExpElem + 100).
			build();
	
	GenSync GenSyncClient = GenSync::Builder().
			setProtocol(GenSync::SyncProtocol::BloomFilterSync).
			setComm(GenSync::SyncComm::socket).
			setBits(BITS).
			setExpNumElems(numExpElem).
			build();
	
	//(oneWay = false, probSync = true, syncParamTest = true, Multiset = false, largeSync = false)
	CPPUNIT_ASSERT(!(syncTest(GenSyncClient, GenSyncServer, false, true, true, false, false)));
}
