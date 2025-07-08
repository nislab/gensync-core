#include <cppunit/extensions/HelperMacros.h>
#include <GenSync/Data/InMemContainer.h>
#include "InMemContainerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(InMemContainerTest);

void InMemContainerTest::setUp(){
    srand(1029);
}

void InMemContainerTest::functionalityTest() {
    //Use a list to compare to the container, as in memory container should function exactly the same.
    list<shared_ptr<DataObject>> objList;
    InMemContainer container;

    
    for(int ii = 0; ii < TIMES; ii++){
        //Initial InMemContainer::empty check
        CPPUNIT_ASSERT_EQUAL(container.empty(), true);
        
        //Fill the lists
        //Tests InMemContainer::push_back
        for(int jj = 0; jj < CONTAINERSIZE; jj++){
            shared_ptr<DataObject> obj = make_shared<DataObject>(randString(LOWER, UPPER));
            container.push_back(obj);
            objList.push_back(obj);
        }
        
        //Second InMemContainer::empty check
        CPPUNIT_ASSERT_EQUAL(container.empty(), false);

        auto listIt = objList.begin();
        auto contIt = container.begin();
        
        //Tests for InMemContainer::push_back validity 
        //Also tests for iterator functions begin(), end(), ++, !=
        for(; contIt != container.end(); contIt++){
            CPPUNIT_ASSERT_EQUAL(*(*contIt) , *(*listIt));
            listIt++;
        }

        //Tests InMemContainer::size
        CPPUNIT_ASSERT_EQUAL(container.size(), objList.size());

        //Tests InMemContainer::remove
        
        listIt = objList.begin();
        for(int jj = 0; jj < 5; jj++){
            container.remove(*listIt);
            listIt++;
        }

        contIt = container.begin();
        for(; contIt != container.end(); contIt++){
            CPPUNIT_ASSERT_EQUAL(*(*contIt) , *(*listIt));
            listIt++;
        }

        //Tests InMemContainer::clear
        container.clear();
        CPPUNIT_ASSERT(container.size() == 0);

        //Reset objList
        objList.clear();
    }

    CPPUNIT_ASSERT(true);
}