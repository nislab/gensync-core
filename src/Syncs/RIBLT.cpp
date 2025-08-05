#include <GenSync/Syncs/RIBLT.h>
#include <iostream>

// ---------------- RIBLT Public ----------------

RIBLT::RIBLT() : valueSize(0), encoder(), decoder() {}

RIBLT::RIBLT(size_t valueSize_) : valueSize(valueSize_), encoder(), decoder() {}

RIBLT::~RIBLT() {}

void RIBLT::insert(ZZ value) {
    auto obj = make_shared<DataObject>(value);
    encoder.insert(obj);
}

CodedSymbol<Symbol> RIBLT::produceNextCell() {
    return encoder.produceNextCell();
}

bool RIBLT::decode(const vector<CodedSymbol<Symbol>>& codedSymbols) {
    decoder.reset();
    for (const auto& cs : codedSymbols) {
        decoder.addCodedSymbol(cs);
    }
    bool success = decoder.tryDecode();

    clientOnly.clear();
    for (const auto& hs : decoder.getClientOnly()) {
        auto wrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(hs.symbol);
        if (wrapper) clientOnly.push_back(wrapper->to_ZZ());
    }

    serverOnly.clear();
    for (const auto& hs : decoder.getServerOnly()) {
        auto wrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(hs.symbol);
        if (wrapper) serverOnly.push_back(wrapper->to_ZZ());
    }

    return success;
}

vector<ZZ> RIBLT::getClientOnly() const {
    return clientOnly;
}

vector<ZZ> RIBLT::getServerOnly() const {
    return serverOnly;
}

void RIBLT::reset() {
    encoder.reset();
    decoder.reset();
    clientOnly.clear();
    serverOnly.clear();
}

// ---------------- Decoder Private ----------------

//void RIBLT::Decoder::applyNewSymbol(const HashedSymbol<Symbol>& hs, int64_t direction) {
//    CodingWindow<Symbol>::RandomMapping m(hs.hash);
//    while (m.lastIdx < codedSymbols.size()) {
//        size_t idx = m.lastIdx;
//        codedSymbols[idx].apply(hs, direction);
//        if ((codedSymbols[idx].getCount() == 1 || codedSymbols[idx].getCount() == -1)
//            && (codedSymbols[idx].getHash() == codedSymbols[idx].getSymbol()->hash())
//            ) {
//            cout << "[Matched]GetHash: "<< codedSymbols[idx].getHash()<< endl;
//            cout << "[Matched]Symbol's Hash: "<< codedSymbols[idx].getSymbol()->hash() << endl;
//            decodable.push_back(static_cast<int>(idx));
//        } else {
//        cout << "[Unmatched] GetHash: "<< codedSymbols[idx].getHash()<< endl;
//        cout << "[Unmatched] Symbol's Hash: "<< codedSymbols[idx].getSymbol()->hash() << endl;}
//        m.nextIndex();
//    }
//}

CodingWindow<Symbol>::RandomMapping RIBLT::Decoder::applyNewSymbol(const HashedSymbol<Symbol>& hs, int64_t direction) {
    CodingWindow<Symbol>::RandomMapping m(hs.hash);

    while (m.lastIdx < codedSymbols.size()) {
        size_t idx = m.lastIdx;
        codedSymbols[idx].apply(hs, direction);

        if ((codedSymbols[idx].getCount() == 1 || codedSymbols[idx].getCount() == -1)
            && (codedSymbols[idx].getHash() == codedSymbols[idx].getSymbol()->hash())) {
            decodable.push_back(static_cast<int>(idx));
        }
        m.nextIndex();
    }

    return m;
}



bool RIBLT::Decoder::tryDecode() {
    for (size_t didx = 0; didx < decodable.size(); ++didx) {
        int cidx = decodable[didx];
        const auto& c = codedSymbols[cidx];

        if (c.getCount() == 1) {
        // Decode remote symbol
        auto sym = c.getSymbol();  // Deep copy
        HashedSymbol<Symbol> hs(sym);
        hs.hash = c.getHash();

        auto mapping = applyNewSymbol(hs, -1);  // Apply to all codedSymbols
        remote.addHashedSymbolWithMapping(hs, mapping);
        decoded++;

        } else if (c.getCount() == -1) {
        // Decode local symbol
        auto sym = c.getSymbol();  // Deep copy
        HashedSymbol<Symbol> hs(sym);
        hs.hash = c.getHash();
        auto mapping = applyNewSymbol(hs, +1);
        local.addHashedSymbolWithMapping(hs, mapping);
        decoded++;

        } else if (c.getCount() == 0) {
        decoded++;
        // This is just noise — possibly already fully peeled
        continue;

        } else {
        std::cerr << "Invalid degree during decoding: " << c.getCount()
                  << std::endl;
        }
    }
    decodable.clear();
    return decoded == codedSymbols.size();
}

