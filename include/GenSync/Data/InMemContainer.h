#ifndef INMEMCONTAINER_H
#define INMEMCONTAINER_H
#include <GenSync/Data/DataContainer.h>
 
/**
 * In-Memory Container that uses a list structure to store information.
 */
class InMemContainer : public DataContainer{
    public:
    /**
     * Specific in-memory iterator. 
     */
    class InMemIterator : public DataIteratorBase {
        public:
            //Uses the iterator of a list internally
            using InnerIter = list<std::shared_ptr<DataObject>>::iterator;

            /**
             * Constructs an iterator with a given iterator.
             * @param it The existing iterator.
             */
            InMemIterator(InnerIter it) : it_(it) {}

            /**
             * Returns the DataObject pointer the iterator is pointing at.
             * @return The DataObject pointer the iterator is pointing at.
             */
            shared_ptr<DataObject> operator*() const override { return *it_; }

            /**
             * Increments the iterator up one element in memory.
             * @return The iterator before the incrementation.
             */
            DataIteratorBase& operator++() override {
                ++it_;
                return *this;
            }
            
            /**
             * Compares two DataIterators by their implementations.
             * @return Whether the iterators point to the same memory address or database position.
             */
            bool operator==(const DataIteratorBase& other) const override {
                // Use dynamic_cast to check type and compare underlying iterators
                auto otherPtr = dynamic_cast<const InMemIterator*>(&other);
                if (!otherPtr) return false;
                return it_ == otherPtr->it_;
            }

            /**
             *  Compares two DataIterators by their implementations.
             *  @return Whether the iterators do not point to the same memory address or database position.
             */
            bool operator!=(const DataIteratorBase& other) const override {
                return !(*this == other);
            }

            /**
             * Returns a pointer to a new iterator that references the same point.
             * @return A copy of a given iterator.
             */
            unique_ptr<DataIteratorBase> clone() const override {
                return unique_ptr<DataIteratorBase>(new InMemIterator(it_));
            }

        private:
            /**Internal Iterator Object*/
            InnerIter it_;
    };

    /**
     * Specific in-memory constant iterator. 
     */
    class ConstInMemIterator : public DataIteratorBase {
        public:
            //Uses the iterator of a list internally
            using InnerIter = list<shared_ptr<DataObject>>::const_iterator;

            /**
             * Constructs an iterator with a given iterator.
             * @param it The existing iterator.
             */
            explicit ConstInMemIterator(InnerIter it) : it_(it) {}

             /**
             * Returns the DataObject pointer the iterator is pointing at.
             * @return The DataObject pointer the iterator is pointing at.
             */
            shared_ptr<DataObject> operator*() const override { return *it_; }

            /**
             * Increments the iterator up one element in memory.
             * @return The iterator before the incrementation.
             */
            DataIteratorBase& operator++() override { ++it_; return *this; }

            /**
             * Compares two DataIterators by their implementations.
             * @return Whether the iterators point to the same memory address or database position.
             */
            bool operator==(const DataIteratorBase& other) const override {
                auto o = dynamic_cast<const ConstInMemIterator*>(&other);
                return o && (it_ == o->it_);
            }
            
            /**
             *  Compares two DataIterators by their implementations.
             *  @return Whether the iterators do not point to the same memory address or database position.
             */
            bool operator!=(const DataIteratorBase& other) const override { return !(*this == other); }
            
            /**
             * Returns a pointer to a new iterator that references the same point.
             * @return A copy of a given iterator.
             */
            unique_ptr<DataIteratorBase> clone() const override {
                return unique_ptr<DataIteratorBase>(new ConstInMemIterator(it_));
            }

        private:
            /**Internal Iterator Object*/
            InnerIter it_;
        };
    
    /**
     * Constructs a default In Memory Container
     */
    InMemContainer() = default;

    /**
     * Default Destructor. Clears items to free memory.
     */
    ~InMemContainer(){clear();}
    
    /**
     * Returns an iterator that points to the beginning of the container.
     * @return An iterator that points to the beginning of the container.
     */
    iterator begin() override;

    /**
     * Returns an iterator that points past the final item of the container.
     * @return An iterator that points past the final item of the container.
     */
    iterator end() override;

     /**
     * Returns a const iterator that points to the beginning of the container.
     * @return A const iterator that points to the beginning of the container.
     */
    const_iterator begin() const override;

     /**
     * Returns a const iterator that points to the beginning of the container.
     * @return A const iterator that points to the beginning of the container.
     */
    const_iterator end() const override;
    
    /**
     * Returns the amount of items inside the container.
     * @return The number of items inside the container.
     */
    size_type size() const override;

    /**
     * Returns whether the container has no items.
     * @return Whether the container has no items.
     */
    bool empty() const override;
    
    /**
     * Removes all items from the container.
     */
    void clear() override;

    /**
     * Removes all items that have the same data as the given DataObject inside the container.
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