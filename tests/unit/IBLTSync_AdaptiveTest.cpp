//
// Created by ChenXingyu on 5/27/25.
//

#include "IBLTSync_AdaptiveTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IBLTSync_AdaptiveTest);

IBLTSync_AdaptiveTest::IBLTSync_AdaptiveTest() = default;

IBLTSync_AdaptiveTest::~IBLTSync_AdaptiveTest() = default;

void IBLTSync_AdaptiveTest::setUp() {
    const int SEED = 93;
    srand(SEED);
}

void IBLTSync_AdaptiveTest::tearDown() {
}

void IBLTSync_AdaptiveTest::IBLTSync_AdaptiveSetReconcileTest() {
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(numExpElem).
            build();

    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(numExpElem).
            build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = false)
    CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true, false, false, false));
}

void IBLTSync_AdaptiveTest::IBLTSync_AdaptiveLargeSetReconcileTest() {
    const int BITS = sizeof(randZZ());

    GenSync GenSyncServer = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(largeNumExpElems).
            build();

    GenSync GenSyncClient = GenSync::Builder().
            setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive).
            setComm(GenSync::SyncComm::socket).
            setBits(BITS).
            setExpNumElems(largeNumExpElems).
            build();

    //(oneWay = false, probSync = true, syncParamTest = false, Multiset = false, largeSync = true)
    CPPUNIT_ASSERT(syncTest(GenSyncClient, GenSyncServer, false, true,false,false,true));
}

void IBLTSync_AdaptiveTest::testAddDelElem() {
    // number of elems to add
    const int ITEMS = 50;
    IBLTSync_Adaptive ibltSync_Adaptive(ITEMS, sizeof(randZZ()));
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(ibltSync_Adaptive.addElem(item));
    }

    // check that elements can be recovered correctly through iterators
    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> resultingElts;
    for(auto iter = ibltSync_Adaptive.beginElements(); iter != ibltSync_Adaptive.endElements(); ++iter)
        resultingElts.insert(*iter);

    vector<shared_ptr<DataObject>> diff;
    rangeDiff(resultingElts.begin(), resultingElts.end(), elts.begin(), elts.end(), back_inserter(diff));
    CPPUNIT_ASSERT(diff.empty());

    // check that delete works
    for(auto dop : elts)
        CPPUNIT_ASSERT(ibltSync_Adaptive.delElem(dop));
}

void IBLTSync_AdaptiveTest::testGetStrings() {
    IBLTSync_Adaptive ibltSync_Adaptive(0, 0);

    CPPUNIT_ASSERT(!ibltSync_Adaptive.getName().empty());
}