/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 2/19/24.
//

#include <GenSync/Syncs/MET_IBLT.h>

MET_IBLT::MET_IBLT() = default;
MET_IBLT::~MET_IBLT() = default;

MET_IBLT::MET_IBLT(const vector<vector<int>>& deg_matrix, 
                   const vector<int>& m_cells, 
                   function<int(ZZ)>& key2type, size_t eltSize)
{
    this->deg_matrix = deg_matrix;
    this->m_cells = m_cells;
    this->key2type = key2type;
    this->eltSize = eltSize;

    for(int cellType = 0; cellType < m_cells.size(); cellType++)
    {
        GenIBLT iblt = GenIBLT::Builder().
                                setValueSize(eltSize).
                                setNumCells(m_cells[cellType]).
                                setCalcNumHashes(createHasher(cellType)).
                                build();
        tables.push_back(iblt);
    }
}

vector<vector<int>> MET_IBLT::getDegMatrix()
{
    return deg_matrix;
}

vector<int> MET_IBLT::getCellTypes()
{
    return m_cells;
}

GenIBLT MET_IBLT::getTable(int cellType)
{
    return tables[cellType];
}

void MET_IBLT::insert(ZZ value)
{
    for(int i = 0; i < tables.size(); i++)
    {
        tables[i].insert(value, value);
    }
}

void MET_IBLT::insert(ZZ value, int cellType)
{
    tables[cellType].insert(value, value);
}

void MET_IBLT::erase(ZZ value)
{
    for(int i = 0; i < tables.size(); i++)
    {
        tables[i].erase(value, value);
    }
}

bool MET_IBLT::peelOnce(std::set<ZZ> &positive, std::set<ZZ> &negative)
{
    positive = {};
    negative = {};
    
    vector<std::set<ZZ>> removedPerTable;
    std::set<ZZ> removedTotal;
    bool success = false;

    for (auto& table : tables)
    {
        vector<pair<ZZ, ZZ>> pos, neg;
        
        // return true if at least one of the IBLT's is able to list entries
        if(table.listEntries(pos, neg))
            success = true;

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

void MET_IBLT::addCellType(int size, vector<int> elemHashes)
{
    m_cells.push_back(size);
    deg_matrix.push_back(elemHashes);
    GenIBLT iblt = GenIBLT::Builder().
                                setValueSize(eltSize).
                                setNumCells(size).
                                setCalcNumHashes(createHasher(m_cells.size()-1)).
                                build();
    tables.push_back(iblt);
}

void MET_IBLT::addGenIBLT(GenIBLT newIBLT)
{
    tables.push_back(newIBLT);
}

function<long(ZZ)> MET_IBLT::createHasher(int cellType)
{
    function<long(ZZ)> hasher = [this, cellType](ZZ elem)
    {
        int elemType = this->key2type(elem);
        long numHashes = deg_matrix[cellType][elemType];
        return numHashes;
    };

    return hasher;
}