/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on April, 2020.
 */

#ifndef COMPACT2DBITARRAYTEST_H
#define COMPACT2DBITARRAYTEST_H

#include <GenSync/Syncs/Compact2DBitArray.h>
#include <cppunit/extensions/HelperMacros.h>

/**
 * Testing parameters
 */
static const size_t MIN_F_SIZE_TESTED = 1,
    MAX_F_SIZE_TESTED = 32,
    MIN_COLUMNS_TESTED = 1,
    MAX_COLUMNS_TESTED = 32, // 64 takes too long
    MIN_ROWS_TESTED = 1,
    MAX_ROWS_TESTED = 32; //64 takes too long

class Compact2DBitArrayTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(Compact2DBitArrayTest);
    CPPUNIT_TEST(readWriteTest);
    CPPUNIT_TEST_SUITE_END();
public:
    Compact2DBitArrayTest();
    ~Compact2DBitArrayTest() override;
    void setUp() override;
    void tearDown() override;

    // test for f bits [MIN_F_SIZE_TESTED..MAX_F_SIZE_TESTED]
    // X columns count [MIN_COLUMNS_TESTED..MAX_COLUMNS_TESTED]
    // X rows count [MIN_ROWS_TESTED..MAX_ROWS_TESTED]
    static void readWriteTest();
};

#endif // COMPACT2DBITARRAYTEST_H
