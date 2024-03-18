/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 2/19/24.
//

#include <GenSync/Syncs/MET_IBLT.h>

MET_IBLT::MET_IBLT() = default;
MET_IBLT::~MET_IBLT() = default;

MET_IBLT::MET_IBLT(const vector<vector<int>>& deg_matrix, 
                   const vector<int>& m_cells, 
                   function<int(ZZ)> key2type, size_t eltSize)
{
    this->deg_matrix = deg_matrix;
    this->m_cells = m_cells;
    this->key2type = key2type;
    this->eltSize = eltSize;

    for(auto numCells: m_cells)
    {
        tables.push_back(IBLT(numCells/1.5, eltSize));

        // Abstraction for new IBLT implementation
        // tables.push_back(IBLT(numCells, eltSize, deg_matrix, key2type)); // no size mult needed
    }
}

void MET_IBLT::insert(ZZ value)
{
    for(int i = 0; i < tables.size(); i++)
    {
        tables[i].insert(value, value);

        // Abstraction for new IBLT implementation
        // int elemType = key2type(value);
        // int hashes = deg_matrix[i][elemType]; // maybe hashes can be calculated within IBLT
        // tables[i].insert(value, value, hashes); // new IBLT will allow diff hashes for each insert
    }
}

void MET_IBLT::insert(ZZ value, int mIndex)
{
    for(int i = 0; i < tables.size(); i++)
    {
        tables[mIndex].insert(value, value);

        // Abstraction for new IBLT implementation
        // int elemType = key2type(value);
        // int hashes = deg_matrix[mIndex][elemType]; // maybe hashes can be calculated within IBLT
        // tables[mIndex].insert(value, value, hashes); // new IBLT will allow diff hashes for each insert
    }
}

void MET_IBLT::erase(ZZ value)
{
    for(int i = 0; i < tables.size(); i++)
    {
        tables[i].erase(value, value);

        // Abstraction for new IBLT implementation
        // int elemType = key2type(value);
        // int hashes = deg_matrix[i][elemType]; // maybe hashes can be calculated within IBLT
        // tables[i].erase(value, value, hashes);
    }
}

void MET_IBLT::addCellType(int size, vector<int> elemHashes)
{
    m_cells.push_back(size);
    deg_matrix.push_back(elemHashes);
    // Abstraction for new IBLT implementation
    // tables.push_back(IBLT(size, eltSize, deg_matrix, key2type)); // no size mult needed
}

bool MET_IBLT::peelOnce(std::set<ZZ> &positive, std::set<ZZ> &negative)
{
    positive = {};
    negative = {};
    
    vector<std::set<ZZ>> removedPerTable;
    std::set<ZZ> removedTotal;
    bool success = true;

    for (auto& table : tables)
    {
        vector<pair<ZZ, ZZ>> pos, neg;
        
        // return false if even one of the IBLT's fail to list all entries
        if(!table.listEntries(pos, neg))
            success = false;

        std::set<ZZ> toBeRemoved;

        for(const auto& pair: neg)
        {
            toBeRemoved.insert(pair.first);
            negative.insert(pair.first);
        }

        for(const auto& pair: pos)
        {
            toBeRemoved.insert(pair.first);
            positive.insert(pair.first);
        }

        removedPerTable.push_back(toBeRemoved);
        removedTotal.insert(toBeRemoved.begin(), toBeRemoved.end());
    }

    for (size_t i = 0; i < tables.size(); ++i) 
    {
        for (auto& x : removedTotal) 
        {
            if (removedPerTable[i].find(x) == removedPerTable[i].end()) 
                tables[i].erase(ZZ(x), ZZ(x));
        }
    }

    return success;
}

bool MET_IBLT::peelAll(vector<ZZ> &positive, vector<ZZ> &negative)
{
    positive = {};
    negative = {};
    
    std::set<ZZ> peels;
    std::set<ZZ> pos;
    std::set<ZZ> neg;
    bool success;

    success = peelOnce(pos, neg);
    while(!pos.empty() || !neg.empty())
    {
        for(auto& val: pos)
            positive.push_back(val);

        for(auto& val: neg)
            negative.push_back(val);
        
        // return success result of final iteration
        success = peelOnce(pos, neg);
    }

    return success;
}

string MET_IBLT::toString()
{
    string outStr = "";
    
    for(auto& ibf: tables)
    {
        outStr += ibf.toString();
        outStr += '\n';
        outStr += '\n';
    }
    
    return outStr;
}