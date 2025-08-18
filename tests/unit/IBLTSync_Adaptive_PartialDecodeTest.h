//
// Created by ChenXingyu on 7/1/25.
//

#ifndef GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODETEST_H
#define GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/IBLTSync_Adaptive_PartialDecode.h>
#include "../TestAuxiliary.h"

class IBLTSync_Adaptive_PartialDecodeTest : public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(IBLTSync_Adaptive_PartialDecodeTest);

        CPPUNIT_TEST(IBLTSync_Adaptive_PartialDecodeSetReconcileTest);
        CPPUNIT_TEST(IBLTSync_Adaptive_PartialDecodeLargeSetReconcileTest);
        CPPUNIT_TEST(testAddDelElem);
        CPPUNIT_TEST(testGetStrings);

    CPPUNIT_TEST_SUITE_END();
public:
    IBLTSync_Adaptive_PartialDecodeTest();

    ~IBLTSync_Adaptive_PartialDecodeTest() override;
    void setUp() override;
    void tearDown() override;

    /**
     * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
     * IBLTSync_Adaptive_PartialDecode is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
     * even for a large amount of tests
     */
    void IBLTSync_Adaptive_PartialDecodeSetReconcileTest();

    /**
     * Test reconciliation of large sets using IBLTSync_Adaptive_PartialDecode
     */
    void IBLTSync_Adaptive_PartialDecodeLargeSetReconcileTest();

    /**
     * Test adding and deleting elements
     */
    void testAddDelElem();

    /**
     * Test that printElem() and getName() return some nonempty string
     */
    void testGetStrings();
};

#endif //GENSYNC_IBLTSYNC_ADAPTIVE_PARTIALDECODETEST_H
