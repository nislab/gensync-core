/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
// Created by GregoryFan on 7/10/2025
//
#ifndef DATABASECONTAINER_H
#define DATABASECONTAINER_H
#include <GenSync/Data/DataContainer.h>

/**
 * Implements a generic container that stores DataObject information.
 * Information stored will be based on different database implementations.
 */
class DatabaseContainer : public DataContainer {
    public:
    //aliases
    using DataContainer::iterator;
    using DataContainer::const_iterator;
    using DataContainer::size_type;

    /**
     * Default destructor, meant to be overriden by subclasses.
     */
    virtual ~DatabaseContainer() = default;

    /**
     * @return An iterator that points to the beginning of the container.
     */
    virtual iterator begin() override = 0;

    /**
     * @return An iterator that points past the final item of the container.
     */
    virtual iterator end() override = 0;

    /**
     * @return A const iterator that points to the beginning of the container.
     */
    virtual const_iterator begin() const override = 0;

    /**
     * @return A const iterator that points to the final item of the container.
     */
    virtual const_iterator end() const override = 0;

    /**
     * @return The number of items inside the container.
     */
    virtual size_type size() const override = 0;

    /**
     * @return Whether the container has no DataObjects.
     */
    virtual bool empty() const override = 0;

    /**
     * Removes all DataObjects from the container.
     */
    virtual void clear() override = 0;

    /**
     * Removes the first DataObject that contain the internal data as the given DataObject.
     * The internal data refers to the information that the DataObject represents.
     * @param val The given DataObject.
     * @return Returns true if object is successfully removed.
     */
    virtual bool remove (const shared_ptr<DataObject>& val) override = 0;

    /**
     * Pushes a given DataObject into the container for storage.
     * @param val The given DataObject to store.
     */
    virtual void add (const shared_ptr<DataObject>& val) override = 0;

};
#endif