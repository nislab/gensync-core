/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
//
// Created by Anish Sinha on 2/19/24.
//

#include <climits>
#include "MET_IBLTTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MET_IBLTTest);

MET_IBLTTest::MET_IBLTTest() {
}

MET_IBLTTest::~MET_IBLTTest() {
}

void MET_IBLTTest::setUp() {
    const int SEED = 617;
    srand(SEED);

    ZZ NTL_SEED = ZZ(617);
    SetSeed(NTL_SEED);
}

void MET_IBLTTest::tearDown() {}

void MET_IBLTTest::testMETBuild()
{
    vector<vector<int>> deg_matrix = {{rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1}};
    vector<int> m_cells = {rand()%3+2, rand()%5+5, rand()%8+9, rand()%8+14};
    function<int(ZZ)> key2type = [] (ZZ key) { return key%4;};
    size_t eltSize = CHAR_BIT;

    MET_IBLT met(deg_matrix, m_cells, key2type, eltSize);

    vector<vector<int>> resMatrix = met.getDegMatrix();
    for(int i = 0; i < 15; i++)
        CPPUNIT_ASSERT(resMatrix[i/4][i%4] == deg_matrix[i/4][i%4]);

    vector<int> resCells = met.getCellTypes();
    for(int i = 0; i < 4; i++)
        CPPUNIT_ASSERT(resCells[i] == m_cells[i]);

    for(int i = 0; i < 4; i++)
        CPPUNIT_ASSERT(met.getTable(i).size() == m_cells[i]);
}

void MET_IBLTTest::testMETInsertPeel()
{
    vector<ZZ> items;
    const int SIZE = 20;

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());
    
    vector<vector<int>> deg_matrix = {{rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1}};
    vector<int> m_cells = {rand()%5+10, rand()%5+20, rand()%5+30, rand()%5+40};
    function<int(ZZ)> key2type = [] (ZZ key) { return key%4;};
    size_t eltSize = sizeof(randZZ());

    MET_IBLT met(deg_matrix, m_cells, key2type, eltSize);

    for(ZZ val: items)
        met.insert(val);

    vector<ZZ> posRes;
    vector<ZZ> negRes;

    CPPUNIT_ASSERT(met.peelAll(posRes, negRes));
    
    CPPUNIT_ASSERT((posRes.size() + negRes.size() == items.size()));

    vector<ZZ> peeledElems;
    for(ZZ val: posRes)
        peeledElems.push_back(val);
    for(ZZ val: negRes)
        peeledElems.push_back(val);

    sort(peeledElems.begin(), peeledElems.end());
    sort(items.begin(), items.end());

    CPPUNIT_ASSERT(peeledElems == items);
}

void MET_IBLTTest::testMETPeelAll()
{
    vector<ZZ> items;
    const int SIZE = 20;

    vector<ZZ> posRes;
    vector<ZZ> negRes;
    vector<ZZ> peeledElems;

    for(int ii = 0; ii < SIZE; ii++)
        items.push_back(randZZ());

    function<int(ZZ)> key2type = [] (ZZ key) { return key%4;};
    size_t eltSize = sizeof(randZZ());

    // Guaranteed Failure
    vector<vector<int>> deg_matrix = {{1,1,1,1}, {2,2,2,2}};
    vector<int> m_cells = {2, 4};
    MET_IBLT metFailure(deg_matrix, m_cells, key2type, eltSize);

    // PeelAll should always succeed when peeling empty MET IBLT
    CPPUNIT_ASSERT(metFailure.peelAll(posRes, negRes));

    for(ZZ val: items)
        metFailure.insert(val);
    
    CPPUNIT_ASSERT(!metFailure.peelAll(posRes, negRes));
    posRes.clear();
    negRes.clear();

    // Guaranteed Success
    deg_matrix = {{rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                  {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                  {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                  {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1}};
    m_cells = {rand()%5+10, rand()%5+20, rand()%5+30, rand()%5+40};
    MET_IBLT metSuccess(deg_matrix, m_cells, key2type, eltSize);

    for(ZZ val: items)
        metSuccess.insert(val);

    CPPUNIT_ASSERT(metSuccess.peelAll(posRes, negRes));
    
    CPPUNIT_ASSERT((posRes.size() + negRes.size() == items.size()));

    for(ZZ val: posRes)
        peeledElems.push_back(val);
    for(ZZ val: negRes)
        peeledElems.push_back(val);

    sort(peeledElems.begin(), peeledElems.end());
    sort(items.begin(), items.end());

    CPPUNIT_ASSERT(peeledElems == items);
}

void MET_IBLTTest::testMETAddCellType()
{
    vector<vector<int>> deg_matrix = {{rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1},
                                      {rand()%4+1,rand()%4+1,rand()%4+1,rand()%4+1}};
    vector<int> m_cells = {rand()%3+2, rand()%5+5, rand()%8+9, rand()%8+14};
    function<int(ZZ)> key2type = [] (ZZ key) { return key%4;};
    size_t eltSize = CHAR_BIT;

    MET_IBLT met(deg_matrix, m_cells, key2type, eltSize);

    int cellSize = rand()%10+10;
    vector<int> elemHashes = {rand()%4+1, rand()%4+1, rand()%4+1, rand()%4+1};
    met.addCellType(cellSize, elemHashes);

    vector<vector<int>> resMatrix = met.getDegMatrix();
    for(int i = 0; i < 4; i++)
        CPPUNIT_ASSERT(resMatrix[4][i] == elemHashes[i]);

    vector<int> resCells = met.getCellTypes();
    CPPUNIT_ASSERT(resCells[4] == cellSize);

    GenIBLT added = met.getTable(4);
    CPPUNIT_ASSERT(added.size() == cellSize);
}
