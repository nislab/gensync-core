/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

// DataFileC.h -- class for data objects container from file
#ifndef DATA_FILE_CONTAINER_H
#define DATA_FILE_CONTAINER_H

#include <GenSync/Auxiliary/Exceptions.h>
#include <GenSync/Auxiliary/Auxiliary.h>
#include "DataObjC.h"

class DataFileC: public DataObjC, public UnimplementedClassException {
public:
    // constructor
    DataFileC() { throw UnimplementedMethodException(); }

    // constructor
    DataFileC(string myfile, string mydir) { throw UnimplementedMethodException(); }

    // destructor
    ~DataFileC() override { throw UnimplementedMethodException(); }

    // read a data object from the file
    void get(shared_ptr<DataObject> mydata, int index) { throw UnimplementedMethodException(); }

    // write a data object into the file
    void put(shared_ptr<DataObject>, int index) { throw UnimplementedMethodException(); }

    // delete a data object by index
    void delObj(int index) override { throw UnimplementedMethodException(); }

    list<shared_ptr<DataObject>> dump() override {
        throw UnimplementedMethodException();
    }

private:
    // the name of the file
    string filename;

    // the directory of the file
    string directory;
};

#endif
