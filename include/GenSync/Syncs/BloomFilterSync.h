/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * The BloomFilter Sync consists of the client and server connecting with each other 
 * and sending each other their respective Bloom filters. The server determines which
 * of its own elements are not present in the client's Bloom filter and sends these
 * elements to the client. The client determiens which of its own elements are not
 * present in the server's Bloom filter and sends these to the server for set reconciliation.
 * 
 * Since Bloom filters have a possibility for false positives, there is a possibility
 * that some elements belonging to the difference set are not reconciled.
 * This probability of failure decreases as the BloomFilter Sync's sizeMultiplier increases.
 *
 * Created by Anish Sinha on 12/4/23.
 */
#ifndef GENSYNCLIB_BLOOMFILTERSYNC_H
#define GENSYNCLIB_BLOOMFILTERSYNC_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Syncs/BloomFilter.h>

class BloomFilterSync : public SyncMethod {
public:
	/**
	 * Constructor.
  	 * @param expected The expected number of elements being stored
	 * @param eltSize The size of elements being stored
	 * @param szMult The size multipier or the size of the Bloom Filter based on number of expected elements
	 * @param nHash The number of hash functions used by the Bloom Filter for each element insertion
    	 */
	BloomFilterSync(size_t expSize, size_t eltSize, int szMult=4, int nHash=3);
	~BloomFilterSync() override;

	// Implemented parent class methods
	bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
	bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;
	bool addElem(shared_ptr<DataObject> datum) override;

	string getName() override;

	/* Getters for parameters of constructor */
	size_t getExpNumElems() const {return expNumElems;}
	size_t getElementSize() const {return elementSize;}

protected:

private:
	
	// Bloom Filter Instance Variable
	BloomFilter myBloomFilter;
	
	// Instance variable to store expected number of elements
	size_t expNumElems;

	// Size of elements as set in constructor
	size_t elementSize;

	// Size Multiplier as set in constructor, determines size of Bloom Filter based on number of expected elements
	int sizeMultiplier;

	// Number of hash functions used by Bloom Filter as set in constructor
	int numHashes;
};

#endif
