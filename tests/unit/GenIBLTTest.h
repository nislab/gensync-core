/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
// 
// Created by tpoimen on 8/1/24
//

#ifndef GENSYNCLIB_GENIBLTTEST_H
#define GENSYNCLIB_GENIBLTTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/GenIBLT.h>
#include <GenSync/Aux/Auxiliary.h>
#include <iostream>
#include <algorithm>

class GenIBLTTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(GenIBLTTest);
    
    CPPUNIT_TEST(testAll);
    CPPUNIT_TEST(SerializeTest);
    CPPUNIT_TEST(IBLTNestedInsertRetrieveTest);

    CPPUNIT_TEST_SUITE_END();

    public:
        GenIBLTTest();
        virtual ~GenIBLTTest();
        void setUp();
        void tearDown();

       /**
 	    * Tests every function in IBLT together, except for size
 	    */
        static void testAll();

        /**
         * Test serilize and de-serialize
         */
        static void SerializeTest();

        /**
         * Test serialize and de-serialize in actual use in IBLT add and list functions
         */
        static void IBLTNestedInsertRetrieveTest();
};

#endif //GENSYNCLIB_GENIBLTTEST_H