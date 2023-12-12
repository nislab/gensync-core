/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 12/4/23.
//

#include <climits>
#include "BloomFilterTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BloomFilterTest);

BloomFilterTest::BloomFilterTest() {
}

BloomFilterTest::~BloomFilterTest() {
}

void BloomFilterTest::setUp() {
    const int SEED = 617;
    srand(SEED);
}

void BloomFilterTest::tearDown() {}

void BloomFilterTest::testBFBuild(){
    int size = rand()%10;
    int nHash = 2 + rand()%5;
    
    BloomFilter bf(size, nHash);

    CPPUNIT_ASSERT(size == bf.getSize());
    CPPUNIT_ASSERT(nHash == bf.getNumHashes());
}

void BloomFilterTest::testBFInsert(){
    vector<ZZ> items;
    const int SIZE = 50;
    const size_t ITEM_SIZE = sizeof(randZZ());

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());

    BloomFilter bf(SIZE*4, 3);
    int inserted = 0;

    for(auto val: items)
    {
        // skip if hash collision
        if(bf.exist(val))
            continue;

        string prev = bf.toString();
        bf.insert(val);
        CPPUNIT_ASSERT(prev != bf.toString());
        inserted++;
    }

    // probabilistically highly likely be true
    CPPUNIT_ASSERT(inserted > SIZE/2);
}

void BloomFilterTest::testBFExist(){
    vector<ZZ> items;
    const int SIZE = 50;
    const size_t ITEM_SIZE = sizeof(randZZ());

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());

    BloomFilter bf(SIZE*2, 3);

    for(auto val: items)
    {
        string prev = bf.toString();
        bool prevExist = bf.exist(val);
        bf.insert(val);

        bool existFtoT = prevExist == false && bf.exist(val) == true;
        bool hashCollision = prev == bf.toString() && prevExist == true;
        CPPUNIT_ASSERT(existFtoT || hashCollision);
    }
}

void BloomFilterTest::testBFBitsZZConv(){
    vector<ZZ> items;
    const int SIZE = 10;
    const size_t ITEM_SIZE = sizeof(randZZ());

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());

    BloomFilter bf(SIZE*2, 3);

    for(auto val: items)
    {
        bf.insert(val);

        long longVal = stoul(bf.toString(), nullptr, 2);
        ZZ ZZVal = bf.toZZ(bf.toString());
        CPPUNIT_ASSERT(longVal == ZZVal);

        string newBitString = bf.ZZtoBitString(ZZVal);
        CPPUNIT_ASSERT(newBitString == bf.toString());
    }
}