/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
// Created by GregoryFan on 7/10/2025
//

#ifdef USE_SQLITE
#ifndef SQLITE_CONTAINER_TEST_H
#define SQLITE_CONTAINER_TEST_H
#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/SQLiteContainer.h>
#include "../TestAuxiliary.h"

class SQLiteContainerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(SQLiteContainerTest);
    CPPUNIT_TEST(addTest);
    CPPUNIT_TEST(clearTest);
    CPPUNIT_TEST(emptyTest);
    CPPUNIT_TEST(sizeTest);
    CPPUNIT_TEST(removeTest);
    CPPUNIT_TEST_SUITE_END();

    public:
    /**
     * The static seed the tests are set to.
     */
    const int SEED = 1920;
    void setUp() override;
    void tearDown() override;

    private:
    unique_ptr<SQLiteContainer> container;
    /**
     * Adds elements to a container and iterates through them using a list
     * as comparison to ensure values have been successfully inserted.
     */
    void addTest();

    /**
     * Tests the clear function by repeatedly filling and clearing the container.
     */
    void clearTest();

    /**
     * Tests the empty function by repeatedly checking when the container is filled and when it is cleared.
     */
    void emptyTest();

    /**
     * Repeatedly fills a container to a random size and compares its size value to the expected amount.
     */
    void sizeTest();

    /**
     * First fills a container, then removes half the elements and compares the other half to a list.
     */
    void removeTest();

};  
#endif //SQLITE_CONTAINER_TEST_H
#endif