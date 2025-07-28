//
// Created by ChenXingyu on 6/18/25.
//
#include <iostream>
#include <NTL/ZZ.h>
#include <GenSync/Syncs/RIBLT.h>
#include <GenSync/Syncs/IBLT.h>
#include <memory>
#include <unistd.h>
#include <GenSync/Syncs/GenSync.h>

using namespace std;
using namespace NTL;

//class IBLTInspector : public IBLT {
//  public:
//    explicit IBLTInspector(const IBLT& iblt) : IBLT(iblt) {}
//
//    const vector<HashTableEntry>& getTable() const {
//        return this->hashTable;
//    }
//};
//
//int main() {
//    cout << "==== RIBLT Test ====" << endl;
//
//    // 创建一个 RIBLT 对象：numHashes=4, hashChecks=11, valueSize=32, ibltSize=50
//    RIBLT riblt(32, 50);
//
//    // 插入若干整数元素
//    vector<int> testValues = {1001, 2002, 3003, 4004, 5005};
//    for (int v : testValues) {
//        ZZ zzval = conv<ZZ>(v);
//        riblt.insert(zzval);
//        cout << "[Insert] Value inserted: " << v << endl;
//    }
//
//    for (int i = 0; i < 5; ++i) {
//        IBLT symbol = riblt.produceNextIBLT();
//        cout << "\n[Coded Symbol " << i << "]" << endl;
//        cout << "  Size: " << symbol.size() << ", Value Size: " << symbol.eltSize() << endl;
//
//        // 输出每个非空 cell 的内容
//        IBLTInspector inspector(symbol);
//        const auto& table = inspector.getTable();
//
//        for (size_t idx = 0; idx < table.size(); ++idx) {
//            const auto& cell = table[idx];
//            if (cell.count != 0 || !IsZero(cell.keySum) || !IsZero(cell.valueSum)) {
//                cout << "Cell[" << idx << "] - "
//                     << "Count: " << cell.count
//                     << ", KeySum: " << cell.keySum
//                     << ", ValueSum: " << cell.valueSum
//                     << ", HashCheck: " << cell.keyCheck << endl;
//            }
//        }
//    }
//
//    cout << "\n==== End of Test ====" << endl;
//    return 0;
//}

int main() {
    size_t eltSize = 8;
    size_t initialExpected = 5;

//    shared_ptr<SyncMethod> syncA = make_shared<R>(initialExpected, eltSize);
//    shared_ptr<SyncMethod> syncB = make_shared<RIBLT>(initialExpected, eltSize);

    GenSync host1 = GenSync::Builder()
                        .setComm(GenSync::SyncComm::socket)
                        .setProtocol(GenSync::SyncProtocol::RIBLTSync)
                        .setBits(8)
                        .build();

    GenSync host2 = GenSync::Builder()
                        .setComm(GenSync::SyncComm::socket)
                        .setProtocol(GenSync::SyncProtocol::RIBLTSync)
                        .setBits(8)
                        .build();

    for (int i = 1; i < 1000; ++i) {
        string elem = to_string(i);
        if (i < 50) {
            // insert [0,19] to both sides
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>(elem));
        } else {
            // insert [20,29] to host1, [30,39] to host2
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>("e" + to_string(i+950)));
        }
    }
    std::cout << "Elements added." << std::endl;

    if (fork()) {
        sleep(1);
        host1.clientSyncBegin(0);

        //        cout << "[Client] Host1 after sync: ";
        //        for (auto &i : host1.dumpElements())
        //            cout << i << " ";
        //        cout << endl;
        cout << "[Client] Host1 element count after sync: " << host1.dumpElements().size() << endl;
        std::cout << "==== Client Statistics ====" << std::endl;
                std::cout << "Communication Time: " << host1.getCommTime(0) << " s" << std::endl;
                std::cout << "Idle Time: " << host1.getIdleTime(0) << " s" << std::endl;
                std::cout << "Computation Time: " << host1.getCompTime(0) << " s" << std::endl;
        std::cout << "Communication Cost (bytes): "
                  << host1.getXmitBytes(0) + host1.getRecvBytes(0) << std::endl;
    } else {
        host2.serverSyncBegin(0);

        //        std::cout << "[Server] Host2 after sync: ";
        //        for (auto &i : host2.dumpElements())
        //            std::cout << i << " ";
        //        std::cout << std::endl;
        cout << "[Server] Host2 element count after sync: " << host2.dumpElements().size() << endl;
        std::cout << "==== Server Statistics ====" << std::endl;
                std::cout << "Communication Time: " << host2.getCommTime(0) << " s" << std::endl;
                std::cout << "Idle Time: " << host2.getIdleTime(0) << " s" << std::endl;
                std::cout << "Computation Time: " << host2.getCompTime(0) << " s" << std::endl;
        std::cout << "Communication Cost (bytes): "
                  << host2.getXmitBytes(0) + host2.getRecvBytes(0) << std::endl;
    }


    return 0;
}
