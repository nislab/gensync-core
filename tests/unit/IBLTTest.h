/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
//
// Created by eliez on 7/17/2018.
//

#ifndef CPISYNCLIB_IBLTTEST_H
#define CPISYNCLIB_IBLTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/IBLT.h>
#include <GenSync/Syncs/IBLTMultiset.h>
#include <GenSync/Aux/Auxiliary.h>
#include <iostream>
#include <algorithm>

class IBLTTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(IBLTTest);
    CPPUNIT_TEST(testAll);
    CPPUNIT_TEST(ListEntriesandKeysTest);
    CPPUNIT_TEST(SerializeTest);
    CPPUNIT_TEST(IBLTNestedInsertRetrieveTest);
    CPPUNIT_TEST(testIBLTMultisetInsert);
    CPPUNIT_TEST(testIBLTMultisetSubtract);

    CPPUNIT_TEST_SUITE_END();
public:
    IBLTTest();
    virtual ~IBLTTest();
    void setUp();
    void tearDown();

	/**
 	* Tests every function in IBLT together, except for size
 	*/
    static void testAll();

    /**
 	* Tests ListEntriesandKeys function in IBLT
 	*/
    static void ListEntriesandKeysTest();

    /**
     * Test serilize and de-serialize
     * */
    static void SerializeTest();

    /**
     * Test serialize and de-serialize in actual use in IBLT add and list functions
     * */
    static void IBLTNestedInsertRetrieveTest();

    /**
     * Test multiset insert into IBLT
     */
    static void testIBLTMultisetInsert();

    /**
     * Test multiset IBLT subtract
     */
    static void testIBLTMultisetSubtract();


};


#endif //CPISYNCLIB_IBLTTEST_H
