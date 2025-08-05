//
// Created by ChenXingyu on 6/13/25.
//

#ifndef GENSYNC_RIBLT_H
#define GENSYNC_RIBLT_H


#include <vector>
#include <queue>
#include <cmath>
#include <memory>
#include <unordered_set>
#include <NTL/ZZ.h>
#include <GenSync/Data/DataObject.h>

using namespace std;
using namespace NTL;

// ---------------- Symbol Interface ----------------

class Symbol {
  public:
    virtual ~Symbol() = default;
    virtual shared_ptr<Symbol> XOR(const shared_ptr<Symbol>& other) const = 0;
    virtual uint64_t hash() const = 0;
    virtual shared_ptr<Symbol> clone() const = 0;
};

class DataObjectSymbolWrapper : public Symbol {
  public:
    explicit DataObjectSymbolWrapper(shared_ptr<DataObject> obj) : data(obj) {}

    shared_ptr<Symbol> XOR(const shared_ptr<Symbol>& other) const override {
        auto otherWrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(other);
        ZZ result = this->data->to_ZZ() ^ otherWrapper->data->to_ZZ();
        return make_shared<DataObjectSymbolWrapper>(make_shared<DataObject>(result));
    }

    uint64_t hash() const override {
        std::hash<string> shash;
        return shash(toStr(data->to_ZZ()));
    }

    shared_ptr<Symbol> clone() const override {
        return make_shared<DataObjectSymbolWrapper>(make_shared<DataObject>(data->to_ZZ()));
    }

    ZZ to_ZZ() const {
        return data->to_ZZ();
    }

  private:
    shared_ptr<DataObject> data;
};

// ---------------- Hashed & Coded Symbol ----------------

template<typename T>
class HashedSymbol {
  public:
    shared_ptr<T> symbol;
    uint64_t hash;

    HashedSymbol() : symbol(nullptr), hash(0) {}
    HashedSymbol(const shared_ptr<T>& s) : symbol(s), hash(s->hash()) {}
    HashedSymbol(const shared_ptr<T>& sym, uint64_t h) : symbol(sym), hash(h) {}
};

template<typename T>
class CodedSymbol {
  public:
    HashedSymbol<T> payload;
    int64_t count;

    CodedSymbol() : payload(), count(0) {}
    CodedSymbol(const shared_ptr<T>& s) : payload(s), count(1) {}
    CodedSymbol(const shared_ptr<T>& s, int64_t c) : payload(s), count(c) {}
    CodedSymbol(const HashedSymbol<T>& hs, int64_t c) : payload(hs), count(c) {}

    void apply(const HashedSymbol<T>& s, int64_t direction) {

        if (!payload.symbol) {
            payload.symbol = s.symbol->clone();
            payload.hash = s.hash;
        } else {
            payload.symbol = payload.symbol->XOR(s.symbol);
            payload.hash ^= s.hash;
        }
        count += direction;
    }

    int64_t getCount() const { return count; }
    shared_ptr<T> getSymbol() const { return payload.symbol; }
    uint64_t getHash() const { return payload.hash; }
    bool isEmpty() const { return count == 0; }
};

// ---------------- Coding Window ----------------

template<typename T>
class CodingWindow {
  public:
    struct SymbolMapping {
        size_t sourceIdx;
        size_t codedIdx;
        bool operator>(const SymbolMapping& other) const {
            return codedIdx > other.codedIdx;
        }
    };

    struct RandomMapping {
        uint64_t prng;
        size_t lastIdx;

        explicit RandomMapping(uint64_t seed) : prng(seed), lastIdx(0) {}

        size_t nextIndex() {
            prng *= 0xda942042e4dd58b5ULL;
            lastIdx += static_cast<size_t>(
                ceil((static_cast<double>(lastIdx) + 1.5) *
                     ((static_cast<double>(1ULL << 32) / sqrt(static_cast<double>(prng) + 1)) - 1.0)));
            return lastIdx;
        }
    };

    void reset() {
        symbols.clear();
        mappings.clear();
        while (!mappingHeap.empty()) mappingHeap.pop();
        nextIdx = 0;
    }

    void addSymbol(const shared_ptr<T>& s) {
        HashedSymbol<T> hs(s);
        addHashedSymbol(hs);
    }

    void addHashedSymbol(const HashedSymbol<T>& hs) {
        RandomMapping m = RandomMapping(hs.hash);
        addHashedSymbolWithMapping(hs, m);
    }

    void addHashedSymbolWithMapping(const HashedSymbol<T>& hs, const RandomMapping& m) {
        symbols.push_back(hs);
        mappings.push_back(m);
        mappingHeap.push(SymbolMapping{symbols.size() - 1, m.lastIdx});
    }

    CodedSymbol<T> applyWindow(CodedSymbol<T> cw, int64_t direction) {
        if (mappingHeap.empty()) {
            nextIdx++;
            return cw;
        }
        while (!mappingHeap.empty() && mappingHeap.top().codedIdx == nextIdx) {
            auto m = mappingHeap.top();
            mappingHeap.pop();

            cw.apply(symbols[m.sourceIdx], direction);
            size_t next = mappings[m.sourceIdx].nextIndex();
            mappingHeap.push({m.sourceIdx, next});
        }

        nextIdx++;
        return cw;
    }

    vector<HashedSymbol<T>> symbols;
    vector<RandomMapping> mappings;
    priority_queue<SymbolMapping, vector<SymbolMapping>, greater<SymbolMapping>> mappingHeap;
    size_t nextIdx = 0;
};

// ---------------- RIBLT Class ----------------

class RIBLT {
  public:
    RIBLT();
    explicit RIBLT(size_t valueSize);
    ~RIBLT();

    void insert(ZZ value);
    CodedSymbol<Symbol> produceNextCell();
    bool decode(const vector<CodedSymbol<Symbol>>& codedSymbols);

    vector<ZZ> getClientOnly() const;
    vector<ZZ> getServerOnly() const;
    void reset();

    // ---------------- Encoder Class ----------------
    class Encoder {
      public:
        void reset() { window.reset(); }
        void insert(const shared_ptr<DataObject>& obj) {
            auto wrapped = make_shared<DataObjectSymbolWrapper>(obj);
            window.addSymbol(wrapped);
        }

        void addSymbol(const shared_ptr<Symbol>& s) {
            window.addSymbol(s);
        }

        void addHashedSymbol(const HashedSymbol<Symbol>& hs) {
            window.addHashedSymbol(hs);
        }

        CodedSymbol<Symbol> produceNextCell() {
            return window.applyWindow(CodedSymbol<Symbol>(), 1);
        }

//      private:
        CodingWindow<Symbol> window;
    };

    // ---------------- Decoder Class ----------------
    class Decoder {
      public:
        Decoder() : decoded(0) {}

        void reset() {
            codedSymbols.clear();
            decodable.clear();
            window.reset();
            local.reset();
            remote.reset();
            decoded = 0;
        }

        void insert(const shared_ptr<DataObject>& obj) {
            auto wrapped = make_shared<DataObjectSymbolWrapper>(obj);
            window.addSymbol(wrapped);
        }

        bool decodedAll() const {
            return decoded == codedSymbols.size();
        }

        vector<HashedSymbol<Symbol>> getClientOnly() const {
            return local.symbols;
        }

        vector<HashedSymbol<Symbol>> getServerOnly() const {
            return remote.symbols;
        }

        vector<ZZ> getOMS() const {
            vector<ZZ> result;
            for (const auto& hs : local.symbols) {
                auto wrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(hs.symbol);
                result.push_back(wrapper->to_ZZ());
            }
            return result;
        }

        vector<ZZ> getSMO() const {
            vector<ZZ> result;
            for (const auto& hs : remote.symbols) {
                auto wrapper = dynamic_pointer_cast<DataObjectSymbolWrapper>(hs.symbol);
                result.push_back(wrapper->to_ZZ());
            }
            return result;
        }


        void addSymbol(const shared_ptr<Symbol>& s) {
            HashedSymbol<Symbol> hs(s);
            addHashedSymbol(hs);
        }

        void addHashedSymbol(const HashedSymbol<Symbol>& hs) {
            window.addHashedSymbol(hs);
        }

        void addCodedSymbol(CodedSymbol<Symbol> c) {
            // Scan through decoded symbols to peel off matching ones

            c = window.applyWindow(c, -1);

            c = remote.applyWindow(c, -1);

            c = local.applyWindow(c, +1);
            codedSymbols.push_back(c);

            if ((c.getCount() == 1 || c.getCount() == -1) && c.getSymbol()->hash() == c.getHash()) {
                decodable.push_back(static_cast<int>(codedSymbols.size()) - 1);
            } else if (c.getCount() == 0 && c.getHash() == 0) {
                decodable.push_back(static_cast<int>(codedSymbols.size()) - 1);
            }
        }

        bool tryDecode();  // Implemented in riblt.cpp

      private:
        CodingWindow<Symbol> window;  // Initial decoder side window
        CodingWindow<Symbol> local;   // Symbols only in decoder
        CodingWindow<Symbol> remote;  // Symbols only in encoder

        vector<CodedSymbol<Symbol>> codedSymbols;
        vector<int> decodable;
        int decoded;

        CodingWindow<Symbol>::RandomMapping applyNewSymbol(const HashedSymbol<Symbol>& hs, int64_t direction);  // Implemented in riblt.cpp
    };


    Encoder encoder;
    Decoder decoder;
    Encoder& getEncoder() { return encoder; }
    Decoder& getDecoder() { return decoder; }
  private:
    size_t valueSize;
    vector<ZZ> clientOnly;
    vector<ZZ> serverOnly;
};

#endif // GENSYNC_RIBLT_H
