/* This code is part of the CPISync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on July, 2020.
 */

#ifndef BENCHPARAMS_H
#define BENCHPARAMS_H

#include <iostream>
#include <GenSync/Syncs/GenSync.h>
#include <GenSync/Benchmarks/DataObjectGenerator.h>
#include <GenSync/Benchmarks/FromFileGen.h>
#include <GenSync/Aux/Sketches.h>

/**
 * Virtual class to represent the parameters of the syncs.
 *
 * For each sync there is a specialization of this class.
 * See the corresponding sync classes for the parameters documentation.
 */
struct Params {
    virtual ostream& serialize(ostream& os) const = 0;
    virtual istream& unserialize(istream& is) = 0;

    friend ostream& operator<<(ostream& os, const Params& x) {return x.serialize(os);}
    friend istream& operator>>(istream& is, Params& x) {return x.unserialize(is);}

    /**
     * Sets up the GenSync::Builder object using this parameters
     * @param gsb The builder object to be used
     */
    virtual void apply(GenSync::Builder& gsb) const = 0;
};

struct FullSyncParams : Params {
    ostream& serialize(ostream& os) const {os << "FullSync\n"; return os;};
    istream& unserialize(istream& is) {string line; getline(is, line); return is;};
    void apply(GenSync::Builder& gsb) const {};
};

struct CPISyncParams : Params {
    size_t m_bar;
    size_t bits;
    float epsilon;
    bool hashes;
    size_t partitions; // InterCPISync has pFactor.
                       // However, it is set via numParts in GenSync so we don't need to
                       // keep separate track of it.

    CPISyncParams() : m_bar (0), bits (0), epsilon (0), hashes (false), partitions (0) {}
    CPISyncParams(size_t m_bar, size_t bits, float epsilon, bool hashes,
                  size_t partitions = 0) :
        m_bar (m_bar), bits (bits), epsilon (epsilon), hashes (hashes),
        partitions (partitions) {}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

struct IBLTParams : Params {
    size_t expected, eltSize, numElemChild;

    IBLTParams() : expected (0), eltSize (0), numElemChild (0) {}
    IBLTParams(size_t expected, size_t eltSize, size_t numElemChild = 0) :
        expected (expected), eltSize (eltSize), numElemChild (numElemChild) {}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

struct BloomFilterParams : Params {
    size_t expected, eltSize;
    float falsePosProb;

    BloomFilterParams() : expected (0), eltSize (0), falsePosProb (0) {}
    BloomFilterParams(size_t expected, size_t eltSize, float falsePosProb = 0.05) :
        expected (expected), eltSize (eltSize), falsePosProb (falsePosProb) {}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

struct MET_IBLTParams : Params {
    size_t eltSize;

    MET_IBLTParams() : eltSize (0){}
    MET_IBLTParams(size_t eltSize) :
        eltSize (eltSize){}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

struct CuckooParams : Params {
    size_t fngprtSize, bucketSize, filterSize, maxKicks;

    CuckooParams() : fngprtSize (0), bucketSize (0), filterSize (0), maxKicks (0) {}
    CuckooParams(size_t fngprtSize, size_t bucketSize, size_t filterSize, size_t maxKicks) :
        fngprtSize (fngprtSize), bucketSize (bucketSize), filterSize (filterSize), maxKicks (maxKicks) {}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

struct RIBLTParams : Params {
    size_t eltSize;

    RIBLTParams() : eltSize (0){}
    RIBLTParams(size_t eltSize) :
                                     eltSize (eltSize){}

    ostream& serialize(ostream& os) const;
    istream& unserialize(istream& is);

    void apply(GenSync::Builder& gsb) const;
};

/**
 * Class that holds the parameters and the data sets for the syncs.
 */
struct BenchParams {
    static const char KEYVAL_SEP;      // separates keys and values in parameter files
    static const string FILEPATH_SEP;  // path separator for the referenced files in parameter files

    BenchParams() = default;
    ~BenchParams();

    /**
     * Reconstructs the sync parameters from the input stream. The
     * format of the file is as follows:
     *
     * SYNC_METHOD_IDENTIFIER (see GenSync::SyncProtocol)
     * Params
     * BenchParams::DELIM_LINE
     * BASE64_ENCODED_ELEMENTS_OF_A_SET(as DataObjects)
     * BenchParams::DELIM_LINE
     * BASE64_ENCODED_ELEMENTS_OF_A_SET(as DataObjects)
     *
     * @param fName The name of the parameters file.
     */
    explicit BenchParams(const string& fName);

    /**
     * This constructor keeps serverElems and clientElems empty
     * @param meth The sync method from which to obtain parameters.
     */
    explicit BenchParams(SyncMethod& meth);

    friend ostream& operator<<(ostream& os, const BenchParams& bp);

    GenSync::SyncProtocol syncProtocol;
    shared_ptr<Params> syncParams;
    shared_ptr<DataObjectGenerator> AElems;  /** Peer A's elements */
    shared_ptr<DataObjectGenerator> BElems;  /** Peer B's elements */
    Nullable<shared_ptr<Sketches>> sketches; /** Set sketches of the SyncMethod, otherwise nothing. */
};

#endif // BENCHPARAMS_H
