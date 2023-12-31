/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * IBLTSync_HalfRound is a one-way implementation of IBLTSync; only one round of communication
 * is used, and only the server is reconciled.
 *
 * Created by Eliezer Pearl on 8/10/2018.
 */


#ifndef GENSYNCLIB_IBLTSYNC_HALFROUND_H
#define GENSYNCLIB_IBLTSYNC_HALFROUND_H

#include <GenSync/Syncs/IBLT.h>
#include <GenSync/Syncs/IBLTSync.h>

class IBLTSync_HalfRound : public IBLTSync {
public:
    // Duplicate the IBLTSync constructor, but set oneWay to true
    IBLTSync_HalfRound(size_t expected, size_t eltSize) : IBLTSync(expected, eltSize) {
        oneWay = true;
        SyncID = SYNC_TYPE::IBLTSync_HalfRound;
    }

    string getName() override {return IBLTSync::getName() + "   * one-way\n";}
};

#endif //GENSYNCLIB_IBLTSYNC_HALFROUND_H
