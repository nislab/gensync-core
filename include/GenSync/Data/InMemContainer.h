/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
#ifndef INMEMCONTAINER_H
#define INMEMCONTAINER_H
#include <GenSync/Data/DataContainer.h>
 
/**
 * Implements an In-Memory Container that stores DataObject information
 * in the internal form of a list. 
 */
class InMemContainer : public DataContainer{
    public:
    /**
     * Generic Iterator for In-Memory Containers
     * This uses an existing in-memory container's iterator 
     * as its internal implementation.
     * @tparam IterType The type of iterator this class uses for its internal implementation.
     */
    template <typename IterType>
    class InMemIteratorBase : public DataIteratorBase {
        public:
            /**
             * Constructs a iterator based off another iterator.
             * @param it The other iterator.
             */
            explicit InMemIteratorBase(IterType it) : it_(it) {}

            /**
             * @return The DataObject pointer the iterator is pointing at.
             */
            shared_ptr<DataObject> operator*() const override {
                return *it_;
            }
            
            /**
             * Moves the iterator up in the container's memory.
             * Returns the iterator after it has moved positions.
             * @return The iterator after it has moved positions in the container.
             */
            DataIteratorBase& operator++() override {
                ++it_;
                return *this;
            }
            
            /**
             * Compares two iterators.
             * Returns true if they point to the same object.
             * @return Whether the two iterators point to the same object in the container.
             */
            bool operator==(const DataIteratorBase& other) const override {
                auto otherPtr = dynamic_cast<const InMemIteratorBase<IterType>*>(&other);
                return otherPtr && (it_ == otherPtr->it_);
            }
            
            /**
             * Compares two iterators. 
             * Returns true if they do not point to the same object.
             * @return Whether the two iterators do not point to the same object in the container.
             */
            bool operator!=(const DataIteratorBase& other) const override {
                return !(*this == other);
            }
            
            /**
             * Creates a new iterator that points to the same object in the container.
             * The iterator is destroyed when the pointer referring to it is moved or destroyed.
             * @return A new iterator that is identical to this one.
             */
            unique_ptr<DataIteratorBase> clone() const override {
                return unique_ptr<DataIteratorBase>(new InMemIteratorBase<IterType>(it_));
            }

        private:
            /**
             * The implementation of the iterator.
             */
            IterType it_;
    };

    //Namespaces
    using InMemIterator = InMemIteratorBase<list<shared_ptr<DataObject>>::iterator>;
    using ConstInMemIterator = InMemIteratorBase<list<shared_ptr<DataObject>>::const_iterator>;

    /**
     * Constructs a default In Memory Container
     */
    InMemContainer() = default;

    /**
     * Default Destructor. Clears items to free memory.
     */
    ~InMemContainer();
    
    /**
     * @return An iterator that points to the beginning of the container.
     */
    iterator begin() override;

    /**
     * @return An iterator that points to the end of the container.
     */
    iterator end() override;

     /**
     * @return A const iterator that points to the beginning of the container.
     */
    const_iterator begin() const override;

     /**
     * @return A const iterator that points to the end of the container.
     */
    const_iterator end() const override;
    
    /**
     * @return The number of items inside the container.
     */
    size_type size() const override;

    /**
     * @return Whether the container has no items.
     */
    bool empty() const override;
    
    /**
     * Removes all items from the container.
     */
    void clear() override;

    /**
     * Removes all DataObjects that contain the internal data as the given DataObject.
     * The internal data refers to the information that the DataObject represents.
     * @param val The given DataObject.
     */
    void remove (const shared_ptr<DataObject>& val) override;

     /**
     * Pushes a given DataObject into the container for storage.
     * @param val The given DataObject to store.
     */
    void push_back (const shared_ptr<DataObject>& val) override;

    private:
    /** The container in which the data is stored in. */
    list<shared_ptr<DataObject>> myData;
};

#endif