#include <GenSync/Aux/Exceptions.h>
#include "../../include/GenSync/Syncs/BloomFilterSync.h"

BloomFilterSync::BloomFilterSync(size_t expected, size_t eltSize)
{
	myBloomFilter = BloomFilter(5,3);
	
	expNumElems = expected;
	elementSize = eltSize;
}

BloomFilterSync::~BloomFilterSync() = default;

bool BloomFilterSync::SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{

	return false;
}

bool BloomFilterSync::SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf)
{

	return false;
}

bool BloomFilterSync::addElem(shared_ptr<DataObject> datum)
{
	SyncMethod::addElem(datum);
	myBloomFilter.insert(datum->to_ZZ());
	return true;
}

bool BloomFilterSync::delElem(shared_ptr<DataObject> datum)
{
	SyncMethod::delElem(datum);
	// cannot erase from bloom filter
	return true;
}

string BloomFilterSync::getName()
{
	return "BloomFilterSync\n   * expected number of elements = " + toStr(expNumElems) + "\n   * size of values =  " + toStr(elementSize) + '\n';
}
