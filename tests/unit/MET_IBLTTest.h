/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 2/19/24.
//

#ifndef CPISYNCLIB_METIBLTTest_H
#define CPISYNCLIB_METIBLTTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Syncs/MET_IBLT.h>
#include <GenSync/Aux/Auxiliary.h>
#include <iostream>
#include <algorithm>

class MET_IBLTTest : public CPPUNIT_NS::TestFixture{
    CPPUNIT_TEST_SUITE(MET_IBLTTest);
    CPPUNIT_TEST(testMETBuild);
    CPPUNIT_TEST(testMETInsertPeel);
    CPPUNIT_TEST(testMETPeelAll);
    CPPUNIT_TEST(testMETAddCellType);

    CPPUNIT_TEST_SUITE_END();
public:
    MET_IBLTTest();
    virtual ~MET_IBLTTest();
    void setUp();
    void tearDown();

    /**
     * Test constructor, setters, and getters of MET_IBLT
     */
    static void testMETBuild();

    /**
     * Test insert and peeling of MET_IBLT
     */
    static void testMETInsertPeel();

    /**
     * Test PeelAll for both success and failure
     */
    static void testMETPeelAll();

    /**
     * Test adding cell type to MET_IBLT
     */
    static void testMETAddCellType();

};

#endif //CPISYNCLIB_MET_IBLTTest_H