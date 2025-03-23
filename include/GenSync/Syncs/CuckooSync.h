/* This code is part of the GenSync project developed at Boston
 * University. Please see the README for use and references.
 *
 * @author Novak Boškov <boskov@bu.edu>
 *
 * Created on Mar, 2020.
 *
 * Implementation of Cuckoo filters based on
 * https://www.cs.cmu.edu/~dga/papers/cuckoo-conext2014.pdf
 */

#ifndef GENSYNCLIB_CUCKOOSYNC_H
#define GENSYNCLIB_CUCKOOSYNC_H

#include <GenSync/Auxiliary/SyncMethod.h>
#include <GenSync/Auxiliary/Auxiliary.h>
#include <GenSync/Syncs/Cuckoo.h>

class CuckooSync : public SyncMethod {
public:
    CuckooSync(size_t fngprtSize, size_t bucketSize,
               size_t filterSize, size_t maxKicks);

    ~CuckooSync() override;

    bool SyncClient(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>>& selfMinusOther,
                    list<shared_ptr<DataObject>>& otherMinusSelf) override;

    bool SyncServer(const shared_ptr<Communicant>& commSync,
                    list<shared_ptr<DataObject>>& selfMinusOther,
                    list<shared_ptr<DataObject>>& otherMinusSelf) override;

    bool addElem(shared_ptr<DataObject> datum) override;

    string getName() override;

    /* Getters for the parameters set in the constructor */
    size_t getFngprtSize() const {return myCF.getFngprtSize();};
    size_t getBucketSize() const {return myCF.getBucketSize();};
    size_t getFilterSize() const {return myCF.getFilterSize();};
    size_t getMaxKicks() const {return myCF.getMaxKicks();};

private:
    /**
     * Cuckoo filter for reconciliation
     */
    Cuckoo myCF;
};

#endif // GENSYNCLIB_CUCKOOSYNC_H
