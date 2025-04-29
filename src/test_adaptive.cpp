//
// Created by ChenXingyu on 4/29/25.
//

#include <iostream>
#include <memory>
#include <unistd.h>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Syncs/IBLTSync_Adaptive.h>  // 你自定义的 Adaptive 类

using namespace std;

int main() {
    size_t eltSize = 8;
    size_t initialExpected = 10;  // 初始估计 entries 数

    // 创建两个主机的同步器
    shared_ptr<SyncMethod> syncA = make_shared<IBLTSync_Adaptive>(initialExpected, eltSize);
    shared_ptr<SyncMethod> syncB = make_shared<IBLTSync_Adaptive>(initialExpected, eltSize);

    GenSync host1 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive)
            .build();

    GenSync host2 = GenSync::Builder()
            .setComm(GenSync::SyncComm::socket)
            .setProtocol(GenSync::SyncProtocol::IBLTSync_Adaptive)
            .build();

    // 插入 100 个元素
    for (int i = 0; i < 100; ++i) {
        string elem = "e" + to_string(i);
        if (i < 90) {
            // 两边都插入 [0,89]
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>(elem));
        } else {
            // host1 插入 [90,99]（共10个），构成 difference
            host1.addElem(make_shared<DataObject>(elem));
            host2.addElem(make_shared<DataObject>("e" + to_string(i+10)));
        }
    }

    // fork 分为 client/server
    if (fork()) {
        sleep(1);  // 等待服务端就绪
        host1.clientSyncBegin(0);

        cout << "[Client] Host1 after sync: ";
        for (auto &i : host1.dumpElements())
            cout << i << " ";
        cout << endl;
    } else {
        host2.serverSyncBegin(0);

        cout << "[Server] Host2 after sync: ";
        for (auto &i : host2.dumpElements())
            cout << i << " ";
        cout << endl;
    }

    return 0;
}
