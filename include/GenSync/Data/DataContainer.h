/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <GenSync/Data/DataObject.h>

/**
 * Custom iterator built for both in memory and database operations. 
 */
class DataIteratorBase {

    public:
    /**
     * Constructs a default iterator.
     */
    virtual ~DataIteratorBase() = default;

    /**
     * Dereferences iterator to the DataObject pointer it refers to.
     * @return Returns the DataObject pointer the iterator is referencing.
     */
    virtual shared_ptr<DataObject> operator*() const = 0;

    /**
     * Moves the iterator up one element in either memory or in database position.
     * Returns the pointer after the incrementation.
     * @return A pointer after the incrementation is done.
     */
    virtual DataIteratorBase& operator++() = 0;

    /**
     * Compares two DataIteratorBases
     * @param other The other iterator.
     * @return Whether the two DataIteratorBases point to the same memory address.
     */
    virtual bool operator==(const DataIteratorBase& other) const = 0;

    /**
     * Returns true if another iterator does NOT share the same reference.
     * @param other The other iterator.
     * @return Whether the two DataIteratorbases do NOT point to the same memory address.
     */
    virtual bool operator!=(const DataIteratorBase& other) const = 0;

    /**
     * Returns a pointer to a new iterator that references the same point.
     * @return A copy of a given iterator.
     */
    virtual unique_ptr<DataIteratorBase> clone() const = 0;
};

/**
 * Wrapper for DataContainer iterator operations.
 * Maintains some functionalities of built-in in-memory iterator.
 */
class DataIterator {
public:
    //namespaces
    using difference_type = ptrdiff_t;
    using value_type = shared_ptr<DataObject>;
    using pointer = shared_ptr<DataObject>*;
    using reference = shared_ptr<DataObject>&;
    using iterator_category = input_iterator_tag;

    /**
     * Constructs a new DataIterator by moving the reference from the given pointer.
     * @param impl The original pointer.
     */
    DataIterator(unique_ptr<DataIteratorBase> impl) : impl_(std::move(impl)) {}

    /**
     * Constructs a new DataIterator by cloning another Dataiterator's pointer.
     * @param other The DataIterator to copy.
     */
    DataIterator(const DataIterator& other) : impl_(other.impl_ ? other.impl_->clone() : nullptr) {}

    /**
     * Replaces the current pointer as the copy of another DataIterator's pointer.
     * @param other The DataIterator to copy.
     * @return The new copied iterator.
     */
    DataIterator& operator=(const DataIterator& other) {
        if (&other != this) {
            impl_ = other.impl_ ? other.impl_->clone() : nullptr;
        }
        return *this;
    }

    /**
     * Moves the iterator up one element in either memory or in database position.
     * @return The iterator BEFORE the incrementation.
     */
    DataIterator operator++(int) {
        DataIterator temp(*this); 
        ++(*this);              
        return temp;           
    }
    
    /**
     * Returns the DataObject pointer the iterator is pointing at.
     * @return The DataObject pointer the iterator is pointing at.
     */
    shared_ptr<DataObject> operator*() const { return **impl_; }

    /**
     * Increments the iterator up one element in either memory or in database position.
     * @return The iterator AFTER the incrementation.
     */
    DataIterator& operator++() { ++(*impl_); return *this; }

    /**
     * Compares two DataIterators by their implementations.
     * @return Whether the iterators point to the same memory address or database position.
     */
    bool operator==(const DataIterator& other) const {
        if (!impl_ || !other.impl_) return impl_ == other.impl_;
        return *impl_ == *other.impl_;
    }

    /**
     *  Compares two DataIterators by their implementations.
     *  @return Whether the iterators do not point to the same memory address or database position.
     */
    bool operator!=(const DataIterator& other) const { return !(*this == other); }

    private:
    /**
     * The implementation of the pointer.
     */
    unique_ptr<DataIteratorBase> impl_;
    friend class InMemContainer;
};

/**
 * Generic container class to represent the method in which DataObjects are held.
 * Has capabilities to be iterated into and certain other features similar to the list type.
 */
class DataContainer{
    public:
        //namespaces
        using iterator = DataIterator;
        using const_iterator = DataIterator;
        using size_type = list<shared_ptr<DataObject>>::size_type;

        /**
         * Default destructor, meant to be overriden by subclasses.
         */
        virtual ~DataContainer() = default;

        /**
         * Returns an iterator that points to the beginning of the container.
         * @return An iterator that points to the beginning of the container.
         */
        virtual iterator begin() = 0;

        /**
         * Returns an iterator that points past the final item of the container.
         * @return An iterator that points past the final item of the container.
         */
        virtual iterator end() = 0;

        /**
         * Returns a const iterator that points to the beginning of the container.
         * @return A const iterator that points to the beginning of the container.
         */
        virtual const_iterator begin() const = 0;

        /**
         * Returns a const iterator that points to the beginning of the container.
         * @return A const iterator that points to the beginning of the container.
         */
        virtual const_iterator end() const = 0;

        /**
         * Returns the amount of items inside the container.
         * @return The number of items inside the container.
         */
        virtual size_type size() const = 0;

        /**
         * Returns whether the container has no items.
         * @return Whether the container has no items.
         */
        virtual bool empty() const = 0;

        /**
         * Removes all items from the container.
         */
        virtual void clear() = 0;

        /**
         * Removes all items that have the same data as the given DataObject inside the container.
         * @param val The given DataObject.
         */
        virtual void remove (const shared_ptr<DataObject>& val) = 0;

        /**
         * Pushes a given DataObject into the container for storage.
         * @param val The given DataObject to store.
         */
        virtual void push_back (const shared_ptr<DataObject>& val) = 0;
};
#endif