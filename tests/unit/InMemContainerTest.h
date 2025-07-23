#ifndef INMEMCONTAINER_TEST_H
#define INMEMCONTAINER_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/InMemContainer.h>
#include "../TestAuxiliary.h"

class InMemContainerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(InMemContainerTest);
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
    const int SEED = 1029;

    /**
     * The amount of iteration each test goes through.
     */
    const int TEST_ITER = 50;  

    /**
     * The size of the container by default.
     */
    const int CONTAINERSIZE = 20;

    /**
     * The minimum size for a random string assigned as data to a DataObject.
     */
    const int LOWER_BOUND_SIZE = 20;

    /**
     * The maximum size for a random string assigned as data to a DataObject.
     */
    const int UPPER_BOUND_SIZE = 50; 

    /**
     * Sets up the random seed.
     */
    void setUp() override;

    private:

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

#endif