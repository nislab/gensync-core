//
// Created by GregoryFan on 7/10/2025
//

#ifdef USE_SQLITE
#include <cppunit/extensions/HelperMacros.h>
#include "SQLiteContainerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SQLiteContainerTest);

//Sets the random seed
void SQLiteContainerTest::setUp(){
    srand(SEED);
    container = unique_ptr<SQLiteContainer>(new SQLiteContainer("testDB", "testTable"));
}

void SQLiteContainerTest::tearDown(){
    container->clear();
    remove("testDB");
}


void SQLiteContainerTest::addTest(){
    //SQLiteContainer container("testDB");
    CPPUNIT_ASSERT(dataContainerAddTest(*container));
}

void SQLiteContainerTest::clearTest(){
    //SQLiteContainer container("testDB");
    CPPUNIT_ASSERT(dataContainerClearTest(*container));
}

void SQLiteContainerTest::emptyTest(){
    //SQLiteContainer container("testDB");
    CPPUNIT_ASSERT(dataContainerEmptyTest(*container));
}

void SQLiteContainerTest::sizeTest(){
    //SQLiteContainer container("testDB");
    CPPUNIT_ASSERT(dataContainerSizeTest(*container));
}

void SQLiteContainerTest::removeTest(){
    //SQLiteContainer container("testDB");
    CPPUNIT_ASSERT(dataContainerRemoveTest(*container));
}
#endif