/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 */

#include "CuckooSyncTest.h"
#include "../TestAuxiliary.h"
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/CuckooSync.h>

CPPUNIT_TEST_SUITE_REGISTRATION(CuckooSyncTest);

CuckooSyncTest::CuckooSyncTest() = default;

CuckooSyncTest::~CuckooSyncTest() = default;

void CuckooSyncTest::setUp() {
    Cuckoo::seedPRNG(1);
    NTL::SetSeed(conv<ZZ>(1));  // make syncTest populate the sets the same way every time
}

void CuckooSyncTest::tearDown() {}

void CuckooSyncTest::setReconcileTest() {
    const size_t bits = sizeof(randZZ());
    const size_t fngprtSize = 12;
    const size_t bucketSize = 4;
    const size_t filterSize = UCHAR_MAX + 1; // UCHAR_MAX is taken from syncTest
    const size_t maxKicks = 500;

    GenSync server = GenSync::Builder()
        .setProtocol(GenSync::SyncProtocol::CuckooSync)
        .setComm(GenSync::SyncComm::socket)
        .setBits(bits)
        .setFngprtSize(fngprtSize)
        .setBucketSize(bucketSize)
        .setFilterSize(filterSize)
        .setMaxKicks(maxKicks)
        .build();

    GenSync client = GenSync::Builder()
        .setProtocol(GenSync::SyncProtocol::CuckooSync)
        .setComm(GenSync::SyncComm::socket)
        .setBits(bits)
        .setFngprtSize(fngprtSize)
        .setBucketSize(bucketSize)
        .setFilterSize(filterSize)
        .setMaxKicks(maxKicks)
        .build();

    // TODO: ZZ_p::init() has to be called in
    // TestAuxiliary:addElements() before random_ZZ_p() is
    // called. random_ZZ_p() segfaults when modulus not set. Since
    // modulus in NTL is global, some other test work if run all
    // together because the execution trace picks up ZZ_p::init() call
    // before it reaches syncTest helper function.
    ZZ_p::init(randZZ());

    CPPUNIT_ASSERT(syncTest(client, server, false, false, false, false, false));
}

void CuckooSyncTest::testAddDelElement() {
    const int ITEMS = 20;
    CuckooSync cuckoo(8,8,8,8);

    multiset<shared_ptr<DataObject>, cmp<shared_ptr<DataObject>>> elts;

    // check that add works
    for(int ii = 0; ii < ITEMS; ii++) {
        shared_ptr<DataObject> item = make_shared<DataObject>(randZZ());
        elts.insert(item);
        CPPUNIT_ASSERT(cuckoo.addElem(item));
    }

    // check that delete works
    for(const auto& dop : elts) {
        CPPUNIT_ASSERT(cuckoo.delElem(dop));
    }

    CPPUNIT_ASSERT_EQUAL(0l, cuckoo.getNumElem());

}
