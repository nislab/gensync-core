#ifndef GENSYNCLIB_BLOOMFILTERSYNC_H
#define GENSYNCLIB_BLOOMFILTERSYNC_H

#include <GenSync/Aux/SyncMethod.h>
#include <GenSync/Aux/Auxiliary.h>
#include <GenSync/Syncs/BloomFilter.h>

class BloomFilterSync : public SyncMethod {
public:
	/*
	 * Constructor.
     	 * @param expected The expected number of elements being stored
     	 * @param eltSize The size of elements being stored
     	 */
	BloomFilterSync(size_t expected, size_t eltSize, int szMult=4, int nHash=3);
	~BloomFilterSync() override;

	// Parent Class Methods
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
	
	// Instance variable for expected number of elements
	size_t expNumElems;

	// Size of elements from constructor
	size_t elementSize;

	int sizeMultiplier;

	int numHashes;
};

#endif
