//
// Created by ChenXingyu on 5/27/25.
//

#ifndef GENSYNC_IBLTSYNC_ADAPTIVETEST_H
#define GENSYNC_IBLTSYNC_ADAPTIVETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/IBLTSync_Adaptive.h>
#include "../TestAuxiliary.h"

class IBLTSync_AdaptiveTest : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(IBLTSync_AdaptiveTest);

        CPPUNIT_TEST(IBLTSync_AdaptiveSetReconcileTest);
        CPPUNIT_TEST(IBLTSync_AdaptiveLargeSetReconcileTest);
        CPPUNIT_TEST(testAddDelElem);
        CPPUNIT_TEST(testGetStrings);

    CPPUNIT_TEST_SUITE_END();
public:
    IBLTSync_AdaptiveTest();

    ~IBLTSync_AdaptiveTest() override;
    void setUp() override;
    void tearDown() override;

    /**
     * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
     * IBLT Sync is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
     * even for a large amount of tests
     */
    void IBLTSync_AdaptiveSetReconcileTest();

    /**
     * Test reconciliation of large sets using IBLTSync_Adaptive
     */
    void IBLTSync_AdaptiveLargeSetReconcileTest();

    /**
     * Test adding and deleting elements
     */
    void testAddDelElem();

    /**
     * Test that printElem() and getName() return some nonempty string
     */
    void testGetStrings();
};

#endif //GENSYNC_IBLTSYNC_ADAPTIVETEST_H
