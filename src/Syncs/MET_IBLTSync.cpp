/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 3/14/24.
//

#include <GenSync/Aux/Exceptions.h>
#include <GenSync/Syncs/MET_IBLTSync.h>
#include <GenSync/Syncs/MET_IBLT.h>

MET_IBLTSync::MET_IBLTSync(size_t expNumElems, size_t eltSize)
{
    this->expNumElems = expNumElems;
    elementSize = eltSize;

    vector<vector<int>> deg_matrix = {{3,4,2}};
    vector<int> m_cells = {1};

    function<int(ZZ)> key2type = [](ZZ key) {
        vector<float> probMatrix = {0.1959, 0.1904, 0.6137};
        
        std::hash<string> shash;
        uint hashedVal = shash(to_string(to_int(key)));
        
        float randVal = hashedVal / static_cast<float>(UINT_MAX);
        
        if(randVal <= probMatrix[0])
        {
            return 0;
        }
        else if(randVal <= probMatrix[1])
        {
            return 1;
        }
        else
        {
            return 2;
        }
    };

    myMET = MET_IBLT(deg_matrix, m_cells, key2type, eltSize);
}

MET_IBLTSync::~MET_IBLTSync() = default;

bool MET_IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    int mIndex = 0;

    while(true)
    {
        commSync->commSend(myMET.tables[mIndex]);
        bool peelSuccess = commSync->commRecv_int();
        
        if(peelSuccess)
            break;
        
        mIndex++;
        myMET.addCellType(pow(2, mIndex), {1,4,1});
        
        for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
        {
            myMET.insert((**iter).to_ZZ(), mIndex);
        }
    }

    /** TODO: Receive differences from server */
    
    return false;
}

bool MET_IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    MET_IBLT diffMET;
    int mIndex = 0;
    std::set<ZZ> diffs;

    while(true)
    {
        IBLT clientIBLT = commSync->commRecv_IBLT(myMET.m_cells[mIndex], elementSize);
        IBLT diffIBLT = myMET.tables[0] - clientIBLT;
        
        diffMET.tables.push_back(diffIBLT);
        bool peelSuccess = diffMET.peelAll(diffs);
        commSync->commSend(peelSuccess);

        if(peelSuccess)
            break;

        mIndex++;
        myMET.addCellType(pow(2, mIndex), {1,4,1});

        for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
        {
            myMET.insert((**iter).to_ZZ(), mIndex);
        }
    }

    /** TODO: Resolve differences and send to client */
    
    return false;
}

bool MET_IBLTSync::addElem(shared_ptr<DataObject> datum)
{
    // call parent add
    SyncMethod::addElem(datum);
    myMET.insert(datum->to_ZZ());
    return true;
}

bool MET_IBLTSync::delElem(shared_ptr<DataObject> datum)
{
    // call parent delete
    SyncMethod::delElem(datum);
    myMET.erase(datum->to_ZZ());
    return true;
}

string MET_IBLTSync::getName()
{
	return "MET IBLT Sync:   Expected number of elements = " + toStr(expNumElems) + "   Size of values = " + toStr(elementSize) + "\n";
}