#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/InMemContainer.h>
#include "InMemContainerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(InMemContainerTest);

//Sets the random seed
void InMemContainerTest::setUp(){
    srand(SEED);
}

void InMemContainerTest::addTest(){
    InMemContainer container;
    CPPUNIT_ASSERT(dataContainerAddTest(container));
}

void InMemContainerTest::clearTest(){
    InMemContainer container;
    CPPUNIT_ASSERT(dataContainerClearTest(container));
}

void InMemContainerTest::emptyTest(){
    InMemContainer container;
    CPPUNIT_ASSERT(dataContainerEmptyTest(container));
}

void InMemContainerTest::sizeTest(){
    InMemContainer container;
    CPPUNIT_ASSERT(dataContainerSizeTest(container));
}

void InMemContainerTest::removeTest(){
    InMemContainer container;
    CPPUNIT_ASSERT(dataContainerRemoveTest(container));
}