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
    
    BloomFilter bf = BloomFilter::Builder().
            setSize(size).
            setNumHashes(nHash).
            build();

    CPPUNIT_ASSERT(size == bf.getSize());
    CPPUNIT_ASSERT(nHash == bf.getNumHashes());

    float falsePosProb = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    size_t numElems = rand()%10;
    const float maxError = 0.05;

    BloomFilter bf2 = BloomFilter::Builder().
            setNumExpElems(numElems).
            setFalsePosProb(falsePosProb).
            build();

    CPPUNIT_ASSERT(abs(bf2.getFalsePosProb(numElems) - falsePosProb) < maxError);
}

void BloomFilterTest::testBFInsert(){
    vector<ZZ> items;
    const int SIZE = 50;
    const size_t ITEM_SIZE = sizeof(randZZ());

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());

    BloomFilter bf = BloomFilter::Builder().
            setSize(SIZE*4).
            setNumHashes(3).
            build();
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

    BloomFilter bf = BloomFilter::Builder().
            setSize(SIZE*2).
            setNumHashes(3).
            build();

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

    BloomFilter bf = BloomFilter::Builder().
            setSize(SIZE*2).
            setNumHashes(3).
            build();

    for(auto val: items)
    {
        bf.insert(val);

        long longVal = stoul(bf.toString(), nullptr, 2);
        ZZ ZZVal = bf.toZZ();
        CPPUNIT_ASSERT(longVal == ZZVal);

        vector<bool> newBitString = bf.ZZtoBF(ZZVal).getBits();
        
        string strTest = "";
        for(auto v: newBitString)
            strTest += to_string(v);

        CPPUNIT_ASSERT(strTest == bf.toString());
    }
}

void BloomFilterTest::testFalsePosProb(){
    vector<ZZ> present;
    vector<ZZ> absent;
    const int NUM_ELEMS = 10000;
    const float expFalsePosProb = 0.05;
    const float maxError = 0.01;

    for(int ii = 0; ii < NUM_ELEMS; ii++)
        present.push_back(randZZ());
    
    while(absent.size() < NUM_ELEMS)
    {
        ZZ val = randZZ();
        if(find(present.begin(), present.end(), val) == present.end())
            absent.push_back(val);
    }

    BloomFilter bf = BloomFilter::Builder().
            setNumExpElems(NUM_ELEMS).
            setFalsePosProb(expFalsePosProb).
            build();

    for(auto val: present)
        bf.insert(val);
    
    int falsePositives = 0;
    for(auto val: absent)
    {
        if(bf.exist(val))
            falsePositives++;
    }

    float actualFalsePosProb = (float)falsePositives / NUM_ELEMS;
    CPPUNIT_ASSERT(abs(actualFalsePosProb - expFalsePosProb) < maxError);
}
