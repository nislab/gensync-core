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
        // tables.push_back(IBLT(numCells, eltSize, deg_cells, key2type)); // no size mult needed
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

bool MET_IBLT::peelOnce(std::set<ZZ> &result)
{
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
            toBeRemoved.insert(pair.first);

        for(const auto& pair: pos)
            toBeRemoved.insert(pair.first);

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

    result = removedTotal;
    return success;
}

bool MET_IBLT::peelAll(std::set<ZZ> &result)
{
    std::set<ZZ> res;
    std::set<ZZ> peels;
    bool success;

    success = peelOnce(peels);
    while(!peels.empty())
    {
        for(auto& val: peels)
            result.insert(val);
        
        // return success result of final iteration
        success = peelOnce(peels);
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