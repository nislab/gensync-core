/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by tpoimen on 8/1/2024.
//

#include <climits>
#include <functional>
#include "GenIBLTTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(GenIBLTTest);

// CalcNumHashes
long CalcNumHashes(ZZ item) {
    return 4;
}

GenIBLTTest::GenIBLTTest() {}

GenIBLTTest::~GenIBLTTest() {}

void GenIBLTTest::setUp()
{
    const int SEED = 617;
    srand(SEED);
}

void GenIBLTTest::tearDown() {}

void GenIBLTTest::testAll()
{
    vector <pair<ZZ, ZZ>> items;
    const int SIZE = 50; // should be even
    const size_t ITEM_SIZE = sizeof(randZZ());

    for (int ii = 0; ii < SIZE; ii++) {
        items.push_back({randZZ(), randZZ()});
    }

    function<long(ZZ)> HASHER = CalcNumHashes;

    GenIBLT geniblt = GenIBLT::Builder().
                      setNumCells(SIZE * 2).
                      setNumHashCheck(11).
                      setValueSize(ITEM_SIZE).
                      setCalcNumHashes(HASHER).
                      build();
    
    for(unsigned int ii=0; ii < SIZE/2; ii++)
        geniblt.insert(items.at(ii).first, items.at(ii).second);

    for(unsigned int ii=SIZE/2; ii < SIZE; ii++)
        geniblt.erase(items.at(ii).first, items.at(ii).second);

    for(unsigned int ii=0; ii < SIZE; ii++) {
        GenIBLT genibltCopy(geniblt); // make a copy each time because getting is destructive
        auto pair = items.at(ii);
        ZZ value;
        CPPUNIT_ASSERT(genibltCopy.get(pair.first, value));
        CPPUNIT_ASSERT_EQUAL(pair.second, value);
    }

    vector<pair<ZZ, ZZ>> plus={}, minus={};
    CPPUNIT_ASSERT(geniblt.listEntries(plus, minus));
    CPPUNIT_ASSERT_EQUAL(items.size(), plus.size() + minus.size());
}

void GenIBLTTest::SerializeTest()
{
    vector<pair<ZZ, ZZ>> pos, neg, ref, neg1, pos1;
    const int NUM_ELEM = 50;
    const int NUM_CELLS = 2 * NUM_ELEM;
    const size_t ITEM_SIZE = sizeof(ZZ(0));
    function<long(ZZ)> HASHER = CalcNumHashes;

    GenIBLT geniblt = GenIBLT::Builder().
                      setNumCells(NUM_CELLS).
                      setNumHashCheck(11).
                      setValueSize(ITEM_SIZE).
                      setCalcNumHashes(HASHER).
                      build();
    
    for (int ii = 1; ii < NUM_ELEM; ii++) {
        geniblt.insert(ZZ(ii), ZZ(ii));
        ref.push_back({ZZ(ii), ZZ(ii)});
    }

    string str = geniblt.toString();
    GenIBLT reconstructed_geniblt = GenIBLT::Builder().
                      setNumCells(NUM_CELLS).
                      setNumHashCheck(11).
                      setValueSize(ITEM_SIZE).
                      setCalcNumHashes(HASHER).
                      build();

    reconstructed_geniblt.reBuild(str);

    // Make sure everything's same between original IBLT and reconstructed IBLT
    CPPUNIT_ASSERT_EQUAL(str, reconstructed_geniblt.toString());
    // Make sure basic functions can still be applied to reconstructed one
    CPPUNIT_ASSERT(geniblt.listEntries(pos, neg));
    //CPPUNIT_ASSERT(reconstructed_geniblt.listEntries(pos, neg));
}

void GenIBLTTest::IBLTNestedInsertRetrieveTest()
{
    multiset<shared_ptr<DataObject>> result;
    std::set<ZZ> setZZ;
    const int expEntries = 20;
    const int BYTE = 8;

    GenIBLT InsideIBLT = GenIBLT::Builder().
                setNumHashes(4).
                setNumHashCheck(11).
                setExpectedNumEntries(expEntries).
                setValueSize(BYTE).
                build();

    //Add data to interior IBLT
    for (int ii = 0; ii < expEntries; ii++)
    {
        unsigned long before = setZZ.size();
        ZZ data = randZZ();
        setZZ.insert(data);
        while (before == setZZ.size())
        {
            data = randZZ();
            setZZ.insert(data);
        }

        shared_ptr<DataObject> InsideData = make_shared<DataObject>(data);
        result.insert(InsideData);
        InsideIBLT.insert(InsideData->to_ZZ(), InsideData->to_ZZ());
    }

    GenIBLT OutsideIBLT = GenIBLT::Builder().
                setNumHashes(4).
                setNumHashCheck(11).
                setExpectedNumEntries(expEntries).
                setValueSize(BYTE).
                build();

    //Insert the inside IBLT into the outside IBLT
    OutsideIBLT.insert(result, BYTE, expEntries);

    vector<pair<ZZ, ZZ>> pos, neg;
    OutsideIBLT.listEntries(pos, neg);

    //Make sure that the inside IBLT is the same as the decoded inside IBLT
    CPPUNIT_ASSERT_EQUAL(InsideIBLT.toString(), zzToString(pos[0].first));
}
