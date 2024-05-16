/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

//
// Created by Anish Sinha on 3/14/24.
//

#include <GenSync/Aux/Exceptions.h>
#include <GenSync/Syncs/MET_IBLTSync.h>
#include <GenSync/Syncs/MET_IBLT.h>

MET_IBLTSync::MET_IBLTSync(size_t eltSize)
{
    elementSize = eltSize;

    vector<vector<int>> deg_matrix = {{2,4,3}};
    vector<int> m_cells = {5};
    vector<float> probMatrix = {0.1959, 0.1904, 0.6137};

    function<int(ZZ)> key2type = [probMatrix](ZZ key) {
        std::hash<string> shash;
        uint hashedVal = shash(to_string(to_int(key)));
        
        float randVal = hashedVal / static_cast<float>(UINT_MAX);
        
        float sumProb = 0.0;
        for(int i = 0; i < probMatrix.size(); i++)
        {
            sumProb += probMatrix[i];
            if(randVal <= sumProb)
                return i;
        }
        
        return int(probMatrix.size());
    };

    myMET = make_shared<MET_IBLT>(deg_matrix, m_cells, key2type, eltSize);
}

MET_IBLTSync::~MET_IBLTSync() = default;

bool MET_IBLTSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    int mIndex = 0;
    int initSize = myMET->getCellTypes()[0];

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commConnect();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while(true)
    {
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(myMET->getTable(mIndex), true);
        bool peelSuccess = commSync->commRecv_int();
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        mySyncStats.timerStart(SyncStats::COMP_TIME);
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
        mySyncStats.timerEnd(SyncStats::COMP_TIME);
    }

    mySyncStats.timerStart(SyncStats::COMM_TIME);
    list<shared_ptr<DataObject>> newOMS = commSync->commRecv_DataObject_List();
    list<shared_ptr<DataObject>> newSMO = commSync->commRecv_DataObject_List();
    mySyncStats.timerEnd(SyncStats::COMM_TIME);

    mySyncStats.timerStart(SyncStats::COMP_TIME);
    otherMinusSelf.insert(otherMinusSelf.end(), newOMS.begin(), newOMS.end());
    selfMinusOther.insert(selfMinusOther.end(), newSMO.begin(), newSMO.end());
    mySyncStats.timerEnd(SyncStats::COMP_TIME);
    
    return true;
}

bool MET_IBLTSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{
    MET_IBLT diffMET;
    int mIndex = 0;
    int initSize = myMET->getCellTypes()[0];
    
    vector<ZZ> diffsPos;
    vector<ZZ> diffsNeg;

    mySyncStats.timerStart(SyncStats::IDLE_TIME);
    commSync->commListen();
    mySyncStats.timerEnd(SyncStats::IDLE_TIME);

    while(true)
    {
        cout << "----------------" << endl;

        mySyncStats.timerStart(SyncStats::COMM_TIME);
        GenIBLT clientIBLT = commSync->commRecv_GenIBLT(myMET->getCellTypes()[mIndex], elementSize, myMET->getTable(mIndex).getCalcNumHashes());
        mySyncStats.timerEnd(SyncStats::COMM_TIME);
        
        mySyncStats.timerStart(SyncStats::COMP_TIME);
        GenIBLT diffIBLT = myMET->getTable(mIndex) - clientIBLT;
        diffMET.addGenIBLT(diffIBLT);

        MET_IBLT diffCopy = diffMET;
        bool peelSuccess = diffCopy.peelAll(diffsPos, diffsNeg);
        mySyncStats.timerEnd(SyncStats::COMP_TIME);
        
        mySyncStats.timerStart(SyncStats::COMM_TIME);
        commSync->commSend(peelSuccess);
        mySyncStats.timerEnd(SyncStats::COMM_TIME);

        if(peelSuccess) {
            cout << diffIBLT.size() << endl;
            break;
        }

        mySyncStats.timerStart(SyncStats::COMP_TIME);
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
        mySyncStats.timerEnd(SyncStats::COMP_TIME);
    }

    mySyncStats.timerStart(SyncStats::COMP_TIME);
    for(const auto& val : diffsPos) {
        selfMinusOther.push_back(make_shared<DataObject>(val));
    }

    for(const auto& val : diffsNeg) {
        otherMinusSelf.push_back(make_shared<DataObject>(val));
    }
    mySyncStats.timerEnd(SyncStats::COMP_TIME);

    mySyncStats.timerStart(SyncStats::COMM_TIME);
    commSync->commSend(selfMinusOther);
    commSync->commSend(otherMinusSelf);
    mySyncStats.timerEnd(SyncStats::COMM_TIME);

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
	return "MET IBLT Sync:   Size of values = " + toStr(elementSize) + "\n";
}