/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 12/4/23.
//

#ifndef CPISYNCLIB_BloomFilterSYNCTEST_H
#define CPISYNCLIB_BloomFilterSYNCTEST_H


#include <cppunit/extensions/HelperMacros.h>

class BloomFilterSyncTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(BloomFilterSyncTest);
	
	CPPUNIT_TEST(BloomFilterSyncSetReconcileTest);
	CPPUNIT_TEST(BloomFilterSyncLargeSetReconcileTest);
	CPPUNIT_TEST(testAddElem);
	CPPUNIT_TEST(testGetStrings);
	CPPUNIT_TEST(testBloomFilterParamMismatch);
	
	CPPUNIT_TEST_SUITE_END();
public:
	BloomFilterSyncTest();
	
	~BloomFilterSyncTest() override;
	void setUp() override;
	void tearDown() override;
	
	/**
	 * Test reconciliation: This test does have a very small probability of failure (If 0 elements are reconciled) as
	 * BloomFilterSync is a probabilistic sync but with a sufficiently large numExpElems and the current seed this isn't an issue,
	 * even for a large amount of tests
	 */
	void BloomFilterSyncSetReconcileTest();
	
	/**
	 * Test reconciliation of large sets using BloomFilterSync
	 */
	void BloomFilterSyncLargeSetReconcileTest();
	
	/**
	 * Test adding elements
	 */
	void testAddElem();
	
	/**
	* Test that printElem() and getName() return some nonempty string
	*/
	void testGetStrings();
	
	/**
	* Test that IBLT Sync reports failure properly with incompatible sync parameters (Different number of expected elements)
	*/
	void testBloomFilterParamMismatch();
};

#endif //CPISYNCLIB_BloomFilterSYNCTEST_H
