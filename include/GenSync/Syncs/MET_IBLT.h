/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * MET IBLT Description
 *
 * Created by Anish Sinha on 2/19/23.
 */

#ifndef GENSYNCLIB_METIBLT_H
#define GENSYNCLIB_METIBLT_H

#include <vector>
#include <utility>
#include <string>
#include <functional>
#include <NTL/ZZ.h>
#include <sstream>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Data/DataObject.h>
#include <GenSync/Syncs/IBLT.h>

using std::hash;
using std::pair;
using std::string;
using std::stringstream;
using std::vector;
using namespace NTL;

// Shorthand for the hash type
typedef unsigned long int hash_t;

class MET_IBLT
{
public:
    MET_IBLT(){};

    MET_IBLT(const std::vector<std::vector<int>>& deg_matrix, 
             const std::vector<int>& m_cells, 
             std::function<int(ZZ)> key2type, size_t valueSize)
    {
        D = deg_matrix;
        M = m_cells;
        this->key2type = key2type;
        this->valueSize = valueSize;

        for(auto numCells: M)
        {
            tables.push_back(IBLT(numCells/1.5, valueSize));
        }
    };

    void insert(ZZ value)
    {
        for(auto& ibf: tables)
        {
            ibf.insert(value, value);
        }
    };

    void erase(ZZ value)
    {
        for(auto& ibf: tables)
        {
            ibf.erase(value, value);
        }
    };

    std::set<ZZ> peelOnce()
    {
        std::vector<std::set<ZZ>> removedPerTable;
        std::set<ZZ> removedTotal;

        for (auto& table : tables)
        {
            vector<pair<ZZ, ZZ>> pos, neg;
            table.listEntries(pos, neg);

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

        return removedTotal;
    };

    std::set<ZZ> peelAll()
    {
        std::set<ZZ> res;
        std::set<ZZ> peels;

        while(!(peels = peelOnce()).empty())
        {
            for(auto& val: peels)
                res.insert(val);
        }

        return res;
    };

    string toString()
    {
        string outStr = "";
        
        for(auto& ibf: tables)
        {
            outStr += ibf.toString();
            outStr += '\n';
            outStr += '\n';
        }
        
        return outStr;
    };

private:
    // Degree matrix. Cell Types X Elem Types
    vector<std::vector<int>> D;

    // Cells, index is cell type, value is number of cells
    vector<int> M;

    // Returns Element's Type given Element
    function<int(ZZ)> key2type;

    // List of IBLTs according to Degree Matrix and Cells
    vector<IBLT> tables;

    // Size of elements being stored
    size_t valueSize;
};

#endif