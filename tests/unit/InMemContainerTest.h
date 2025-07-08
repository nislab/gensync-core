#ifndef INMEMCONTAINER_TEST_H
#define INMEMCONTAINER_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/SQLiteContainer.h>
#include "../TestAuxiliary.h"

class InMemContainerTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(InMemContainerTest);
    CPPUNIT_TEST(functionalityTest);
    CPPUNIT_TEST_SUITE_END();

    public:
    const int TIMES = 50;   
    const int CONTAINERSIZE = 20;
    const int LOWER = 20;
    const int UPPER = 50; 
    void setUp() override;

    private:
    /*Tests basic list method functionality*/
    void functionalityTest();
};

#endif