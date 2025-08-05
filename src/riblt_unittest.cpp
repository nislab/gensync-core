//
// Created by ChenXingyu on 6/18/25.
//
//
// Created by ChenXingyu on 6/18/25.
//

#include <iostream>
#include <memory>
#include <unordered_set>
#include <GenSync/Syncs/RIBLT.h>
#include <GenSync/Communicants/Communicant.h>

using namespace std;

using namespace std;

//void testCodedSymbolSendRecv() {
//    Communicant comm;
//
//    // 构造一个测试用的 CodedSymbol
//    auto data = make_shared<DataObject>(ZZ(12345));
//    auto symbol = make_shared<DataObjectSymbolWrapper>(data);
//    uint64_t hash = symbol->hash();
//    int64_t count = 3;
//
//    HashedSymbol<Symbol> hs(symbol, hash);
//    CodedSymbol<Symbol> original(hs, count);
//
//    // 使用 commSend 发送 CodedSymbol
//    comm.commSend(original);
//
//    // 使用 commRecv 接收并构造新的 CodedSymbol
//    CodedSymbol<Symbol> received = comm.commRecv_CodedSymbol();
//
//    // 比较值是否一致
//    auto origWrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(original.getSymbol());
//    auto recvWrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(received.getSymbol());
//
//    cout << "Original: ZZ = " << origWrapper->to_ZZ() << ", Hash = " << original.getHash()
//         << ", Count = " << original.getCount() << endl;
//    cout << "Received: ZZ = " << recvWrapper->to_ZZ() << ", Hash = " << received.getHash()
//         << ", Count = " << received.getCount() << endl;
//
//    if (origWrapper->to_ZZ() == recvWrapper->to_ZZ() &&
//        original.getHash() == received.getHash() &&
//        original.getCount() == received.getCount()) {
//        cout << "✅ CodedSymbol transmission test passed!" << endl;
//    } else {
//        cout << "❌ CodedSymbol transmission test failed!" << endl;
//    }
//}

int main() {

//        testCodedSymbolSendRecv();
//        return 0;


        using item  = ZZ;

        vector<item> alice = {ZZ(12), ZZ(42), ZZ(33), ZZ(87), ZZ(58), ZZ(68), ZZ(89), ZZ(81), ZZ(82), ZZ(88)};
        vector<item> bob   = {ZZ(12), ZZ(45),ZZ(55),ZZ(68)};

        // 初始化 encoder 和 decoder
        RIBLT client(128);
        RIBLT server(128);
        auto& encoder = client.getEncoder();
        auto& decoder = server.getDecoder();
        cout << "Test...." << endl;
        // Alice 添加她的元素到 encoder
        for (const auto& v : alice) {
            auto dataObj = make_shared<DataObject>(v);
            auto symbol = make_shared<DataObjectSymbolWrapper>(dataObj);
            encoder.addSymbol(symbol);
        }

        cout << "Encoder Built!" << endl;
        // Bob 添加他的元素到 decoder
        for (const auto& v : bob) {
            auto dataObj = make_shared<DataObject>(v);
            auto symbol = make_shared<DataObjectSymbolWrapper>(dataObj);
            decoder.addSymbol(symbol);
        }
        cout << "Decoder Built!" << endl;

        int cost = 0;
        while (true) {
            // Alice 生成下一个 coded symbol
            auto symbol = encoder.produceNextCell();
            cost++;

            // Bob 接收并处理
            decoder.addCodedSymbol(symbol);
            if (decoder.tryDecode()) break;
        }

        cout << "Decoded successfully!" << endl;

        auto alice_only = decoder.getClientOnly(); // A \ B
        auto bob_only   = decoder.getServerOnly(); // B \ A
        cout << alice_only.size() << endl;
        cout << bob_only.size() << endl;

        // 打印 Alice 专属的元素
        cout << "Elements exclusive to Alice:" << endl;
        for (const auto& hs : alice_only) {
            auto wrapper = std::dynamic_pointer_cast<DataObjectSymbolWrapper>(hs.symbol);
            if (wrapper) {
                std::cout << wrapper->to_ZZ() << std::endl;
            } else {
                std::cerr << "❌ Failed to cast symbol to DataObjectSymbolWrapper" << std::endl;
            }
        }



        cout << cost << " coded symbols sent" << endl;

        return 0;
}





