#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <GenSync/Data/DataObject.h>

/**
 * Custom iterator base for In Memory and Database operations.
 */
class DataIteratorBase {
public:
    virtual ~DataIteratorBase() = default;
    virtual std::shared_ptr<DataObject> operator*() const = 0;
    virtual DataIteratorBase& operator++() = 0;
    virtual bool operator==(const DataIteratorBase& other) const = 0;
    virtual bool operator!=(const DataIteratorBase& other) const = 0;
    virtual std::unique_ptr<DataIteratorBase> clone() const = 0;
};

/**
 * Wrapper for DataContainer iterator operations.
 * Maintains some functionalities of built-in in-memory iterator.
 */
class DataIterator {
public:
    using difference_type = ptrdiff_t;
    using value_type = shared_ptr<DataObject>;
    using pointer = shared_ptr<DataObject>*;
    using reference = shared_ptr<DataObject>&;
    using iterator_category = input_iterator_tag;

    /*Constructs iterator based on an implementation*/
    DataIterator(unique_ptr<DataIteratorBase> impl) : impl_(std::move(impl)) {}

    /*Constructs iterator based on an exisiting iterator*/
    DataIterator(const DataIterator& other) : impl_(other.impl_ ? other.impl_->clone() : nullptr) {}

    //Iterator functions
    DataIterator& operator=(const DataIterator& other) {
        if (&other != this) {
            impl_ = other.impl_ ? other.impl_->clone() : nullptr;
        }
        return *this;
    }

    DataIterator operator++(int) {
        DataIterator temp(*this); 
        ++(*this);              
        return temp;           
    }
    
    shared_ptr<DataObject> operator*() const { return **impl_; }
    DataIterator& operator++() { ++(*impl_); return *this; }

    bool operator==(const DataIterator& other) const {
        if (!impl_ || !other.impl_) return impl_ == other.impl_;
        return *impl_ == *other.impl_;
    }
    bool operator!=(const DataIterator& other) const { return !(*this == other); }

    private:
    unique_ptr<DataIteratorBase> impl_;
    friend class InMemContainer;
};

/**
 * Generic container class to represent the method in which DataObjects are held.
 * Has capabilities to be iterated into and certain other features similar to the list type.
 */
class DataContainer{
    public:
        using iterator = DataIterator;
        using const_iterator = DataIterator;
        using size_type = list<shared_ptr<DataObject>>::size_type;

        //To be overriden by different methods of cleaning.
        virtual ~DataContainer() = default;

        //Iterator Methods
        virtual iterator begin() = 0;
        virtual iterator end() = 0;

        virtual const_iterator begin() const = 0;
        virtual const_iterator end() const = 0;

        //Parameter Getters
        virtual size_type size() const = 0;
        virtual bool empty() const = 0;

        //Manipulation
        virtual void clear() = 0;
        virtual void remove (const shared_ptr<DataObject>& val) = 0;
        virtual void push_back (const shared_ptr<DataObject>& val) = 0;
};
#endif