/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 12/4/23.
//

#ifndef CPISYNCLIB_BloomFilterTest_H
#define CPISYNCLIB_BloomFilterTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/BloomFilter.h>
#include <GenSync/Aux/Auxiliary.h>
#include <iostream>
#include <algorithm>

class BloomFilterTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(BloomFilterTest);
    CPPUNIT_TEST(testBFBuild);
    CPPUNIT_TEST(testBFInsert);
    CPPUNIT_TEST(testBFExist);
    CPPUNIT_TEST(testBFBitsZZConv);

    CPPUNIT_TEST_SUITE_END();
public:
    BloomFilterTest();
    virtual ~BloomFilterTest();
    void setUp();
    void tearDown();

    /**
     * Test constructor, setters, and getters of BloomFilter
     */
    static void testBFBuild();

    /**
     * Test insert into BloomFilter
     */
    static void testBFInsert();

    /**
     * Test BloomFilter exist method
     */
    static void testBFExist();

    /**
     * Test conversions between BloomFilter and ZZ
     */
    static void testBFBitsZZConv();


};

#endif //CPISYNCLIB_BloomFilterTest_H