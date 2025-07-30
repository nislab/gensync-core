#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/InMemContainer.h>
#include "InMemContainerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(InMemContainerTest);

//Sets the random seed
void InMemContainerTest::setUp(){
    srand(SEED);
}

void InMemContainerTest::addIterateTest(){
    //Create containers
    list<shared_ptr<DataObject>> objList;
    InMemContainer container;

    for(int ii = 0; ii < TEST_ITER; ii++){
        //Fill containers
        for(int jj = 0; jj < CONTAINERSIZE; jj++){
            shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER_BOUND_SIZE, UPPER_BOUND_SIZE));
            container.add(obj);
            objList.push_back(obj);
        }
        
        auto listIt = objList.begin();
        auto contIt = container.begin();
        
        //Iterates through both lists
        //Ensures the InMemoryContainer has
        for(; contIt != container.end(); contIt++){
            CPPUNIT_ASSERT_EQUAL(*(*contIt) , *(*listIt));
            listIt++;
        }
    }
    CPPUNIT_ASSERT(true);
}

void InMemContainerTest::clearTest(){
    InMemContainer container;
    for(int ii = 0; ii < TEST_ITER; ii++){
        //Fills container
        shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER_BOUND_SIZE, UPPER_BOUND_SIZE));
        container.add(obj);
        container.clear();
        //Checks if container is empty after clear
        CPPUNIT_ASSERT(container.empty() && container.size() == 0);
    }
    CPPUNIT_ASSERT(true);
}

void InMemContainerTest::emptyTest(){
    InMemContainer container;
    for(int ii = 0; ii < TEST_ITER; ii++){
        //Checks if container is empty when it is empty
        CPPUNIT_ASSERT_EQUAL(true, container.empty());
        shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER_BOUND_SIZE, UPPER_BOUND_SIZE));
        container.add(obj);
         //Checks if container is empty when it is not empty
        CPPUNIT_ASSERT_EQUAL(false, container.empty());
        container.clear();
    }
    CPPUNIT_ASSERT(true);
}

void InMemContainerTest::sizeTest(){
    InMemContainer container;
    for(int ii = 0; ii < TEST_ITER; ii++){
        //Determines a random number between 0 to 99.
        int randSize = rand() % 100;
        //Fills the loop that amount of times
        for(int jj = 0; jj < randSize; jj++){
            shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER_BOUND_SIZE, UPPER_BOUND_SIZE));
            container.add(obj);
        }
        //Compares the two sizes to ensure they are the same.
        CPPUNIT_ASSERT_EQUAL(static_cast<DataContainer::size_type>(randSize),container.size());
        container.clear();
    }
    CPPUNIT_ASSERT(true);
}

void InMemContainerTest::removeTest(){
    //Create containers
    list<shared_ptr<DataObject>> objList;
    InMemContainer container;

    for(int ii = 0; ii < TEST_ITER; ii++){
        //Fill containers
        for(int jj = 0; jj < CONTAINERSIZE; jj++){
            shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER_BOUND_SIZE, UPPER_BOUND_SIZE));
            container.add(obj);
            objList.push_back(obj);
        }

        auto listIt = objList.begin();
        auto contIt = container.begin();

        //Remove half the list from the In Memory Container
        for(int jj = 0; jj < CONTAINERSIZE/2; jj++){
            container.remove(*listIt);
            listIt++;
        }

        contIt = container.begin();

        //Compares the resultant container against the list
        for(; contIt != container.end(); contIt++){
            CPPUNIT_ASSERT_EQUAL(*(*contIt) , *(*listIt));
            listIt++;
        }
        container.clear();
        objList.clear();
    }
    CPPUNIT_ASSERT(true);
}