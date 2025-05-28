//
// Created by ChenXingyu on 5/15/25.
//

#include <iostream>
#include <memory>
#include <unistd.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/MET_IBLTSync.h>

using namespace std;

int main() {
    size_t eltSize = 8;
    size_t initialExpected = 5;

    GenSync host1 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::MET_IBLTSync)
            .setBits(8)
            .build();

    GenSync host2 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::MET_IBLTSync)
            .setBits(8)
            .build();
    std::cout << "Both hosts generated." << std::endl;
    for (int i = 0; i < 1000; ++i) {
        string elem = "e" + to_string(i);
        if (i < 990) {
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

//        cout << "[Client] Host1 after sync: ";
//        for (auto &i : host1.dumpElements())
//            cout << i << " ";
//        cout << endl;
        cout << "[Client] Host1 after sync, element count: "
             << host1.dumpElements().size() << endl;
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
        cout << "[Client] Host2 after sync, element count: "
             << host2.dumpElements().size() << endl;


        std::cout << "==== Server Statistics ====" << std::endl;
        std::cout << "Communication Time: " << host2.getCommTime(0) << " s" << std::endl;
        std::cout << "Idle Time: " << host2.getIdleTime(0) << " s" << std::endl;
        std::cout << "Computation Time: " << host2.getCompTime(0) << " s" << std::endl;
        std::cout << "Communication Cost (bytes): "
                  << host2.getXmitBytes(0) + host2.getRecvBytes(0) << std::endl;
    }


    return 0;
}
