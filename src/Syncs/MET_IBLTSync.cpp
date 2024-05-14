/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 3/14/24.
//

#include <GenSync/Aux/Exceptions.h>
#include <GenSync/Syncs/MET_IBLTSync.h>
#include <GenSync/Syncs/MET_IBLT.h>

MET_IBLTSync::MET_IBLTSync(size_t eltSize)
{
    this->expNumElems = expNumElems;
    elementSize = eltSize;

    vector<vector<int>> deg_matrix = {{2,4,3}};
    vector<int> m_cells = {5};
    vector<float> probMatrix = {0.1959, 0.1904, 0.6137};

    function<int(ZZ)> key2type = [probMatrix](ZZ key) {
        std::hash<string> shash;
        uint hashedVal = shash(to_string(to_int(key)));
        
        float randVal = hashedVal / static_cast<float>(UINT_MAX);
        
        if(randVal <= probMatrix[0])
        {
            return 0;
        }
        if(randVal <= probMatrix[0] + probMatrix[1])
        {
            return 1;
        }
        
        return 2;
        
    };

    myMET = make_shared<MET_IBLT>(deg_matrix, m_cells, key2type, eltSize);
}

MET_IBLTSync::~MET_IBLTSync() = default;

bool MET_IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    int mIndex = 0;
    int initSize = myMET->getCellTypes()[0];
    commSync->commConnect();

    while(true)
    {
        commSync->commSend(myMET->getTable(mIndex), true);
        bool peelSuccess = commSync->commRecv_int();
        
        if(peelSuccess)
            break;
        
        mIndex++;
        
        vector<int> cellMatrix = {4,4,4};
        if(mIndex > 4)
            cellMatrix = {5,5,5};
            
        myMET->addCellType(pow(2, mIndex) * initSize, cellMatrix);
        
        for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
        {
            myMET->insert((**iter).to_ZZ(), mIndex);
        }
    }

    list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
    list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();

    otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
    selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
    
    return true;
}

bool MET_IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    MET_IBLT diffMET;
    int mIndex = 0;
    int initSize = myMET->getCellTypes()[0];
    
    vector<ZZ> diffsPos;
    vector<ZZ> diffsNeg;

    commSync->commListen();

    while(true)
    {
        cout << "----------------" << endl;
        GenIBLT clientIBLT = commSync->commRecv_GenIBLT(myMET->getCellTypes()[mIndex], elementSize, myMET->getTable(mIndex).getCalcNumHashes());
        GenIBLT diffIBLT = myMET->getTable(mIndex) - clientIBLT;
        diffMET.addGenIBLT(diffIBLT);

        MET_IBLT diffCopy = diffMET;
        bool peelSuccess = diffCopy.peelAll(diffsPos, diffsNeg);
        commSync->commSend(peelSuccess);

        if(peelSuccess) {
            cout << diffIBLT.size() << endl;
            break;
        }

        diffsPos.clear();
        diffsNeg.clear();
        
        mIndex++;

        vector<int> cellMatrix = {4,4,4};
        if(mIndex > 4)
            cellMatrix = {5,5,5};

        myMET->addCellType(pow(2, mIndex) * initSize, cellMatrix);

        for(auto iter = SyncMethod::beginElements(); iter != SyncMethod::endElements(); iter++)
        {
            myMET->insert((**iter).to_ZZ(), mIndex);
        }
    }

    for(const auto& val : diffsPos) {
        selfMinusOther.push_back(make_shared<DataObject>(val));
    }

    for(const auto& val : diffsNeg) {
        otherMinusSelf.push_back(make_shared<DataObject>(val));
    }

    commSync->commSend(selfMinusOther);
    commSync->commSend(otherMinusSelf);

    return true;
}

bool MET_IBLTSync::addElem(shared_ptr<DataObject> datum)
{
    // call parent add
    SyncMethod::addElem(datum);
    myMET->insert(datum->to_ZZ());
    return true;
}

bool MET_IBLTSync::delElem(shared_ptr<DataObject> datum)
{
    // call parent delete
    SyncMethod::delElem(datum);
    myMET->erase(datum->to_ZZ());
    return true;
}

string MET_IBLTSync::getName()
{
	return "MET IBLT Sync:   Expected number of elements = " + toStr(expNumElems) + "   Size of values = " + toStr(elementSize) + "\n";
}