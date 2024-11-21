/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 4/11/24.
//

#ifndef CPISYNCLIB_METIBLTSYNCTEST_H
#define CPISYNCLIB_METIBLTSYNCTEST_H


#include <cppunit/extensions/HelperMacros.h>

class MET_IBLTSyncTest : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE(MET_IBLTSyncTest);
	
	CPPUNIT_TEST(MET_IBLTSyncSetReconcileTest);
	CPPUNIT_TEST(MET_IBLTSyncLargeSetReconcileTest);
	CPPUNIT_TEST(testAddElem);
	CPPUNIT_TEST(testGetStrings);
	
	CPPUNIT_TEST_SUITE_END();
public:
	MET_IBLTSyncTest();
	
	~MET_IBLTSyncTest() override;
	void setUp() override;
	void tearDown() override;
	
	/**
	 * Test reconciliation of sets using MET_IBLTSync
	 */
	void MET_IBLTSyncSetReconcileTest();
	
	/**
	 * Test reconciliation of large sets using MET_IBLTSync
	 */
	void MET_IBLTSyncLargeSetReconcileTest();
	
	/**
	 * Test adding elements
	 */
	void testAddElem();
	
	/**
	* Test that printElem() and getName() return some nonempty string
	*/
	void testGetStrings();
};

#endif //CPISYNCLIB_METIBLTSYNCTEST_H
