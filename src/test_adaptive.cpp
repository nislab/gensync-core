//
// Created by ChenXingyu on 4/29/25.
//

#include <iostream>
#include <memory>
#include <unistd.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/IBLTSync_Adaptive.h>

using namespace std;

int main() {
    size_t eltSize = 8;
    size_t initialExpected = 5;

    shared_ptr<SyncMethod> syncA = make_shared<IBLTSync_Adaptive>(initialExpected, eltSize);
    shared_ptr<SyncMethod> syncB = make_shared<IBLTSync_Adaptive>(initialExpected, eltSize);

    GenSync host1 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive)
            .setExpNumElems(2)
            .setBits(8)
            .build();

    GenSync host2 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive)
            .setExpNumElems(2)
            .setBits(8)
            .build();

    for (int i = 0; i < 30; ++i) {
        string elem = "e" + to_string(i);
        if (i < 20) {
            // insert [0,19] to both sides
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>(elem));
        } else {
            // insert [20,29] to host1, [30,39] to host2
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>("e" + to_string(i+10)));
        }
    }
    std::cout << "Elements added." << std::endl;

    if (fork()) {
        sleep(1);
        host1.clientSyncBegin(0);

        cout << "[Client] Host1 after sync: ";
        for (auto &i : host1.dumpElements())
            cout << i << " ";
        cout << endl;
        std::cout << "==== Client Statistics ====" << std::endl;
        std::cout << "Communication Time: " << host1.getCommTime(0) << " s" << std::endl;
        std::cout << "Idle Time: " << host1.getIdleTime(0) << " s" << std::endl;
        std::cout << "Computation Time: " << host1.getCompTime(0) << " s" << std::endl;
        std::cout << "Communication Cost (bytes): "
                  << host1.getXmitBytes(0) + host1.getRecvBytes(0) << std::endl;
    } else {
        host2.serverSyncBegin(0);

        std::cout << "[Server] Host2 after sync: ";
        for (auto &i : host2.dumpElements())
            std::cout << i << " ";
        std::cout << std::endl;

        std::cout << "==== Server Statistics ====" << std::endl;
        std::cout << "Communication Time: " << host2.getCommTime(0) << " s" << std::endl;
        std::cout << "Idle Time: " << host2.getIdleTime(0) << " s" << std::endl;
        std::cout << "Computation Time: " << host2.getCompTime(0) << " s" << std::endl;
        std::cout << "Communication Cost (bytes): "
                  << host2.getXmitBytes(0) + host2.getRecvBytes(0) << std::endl;
    }


    return 0;
}
