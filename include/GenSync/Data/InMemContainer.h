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

            //Constructor
            InMemIterator(InnerIter it) : it_(it) {}

            //Pointer operations
            shared_ptr<DataObject> operator*() const override { return *it_; }

            DataIteratorBase& operator++() override {
                ++it_;
                return *this;
            }

            bool operator==(const DataIteratorBase& other) const override {
                // Use dynamic_cast to check type and compare underlying iterators
                auto otherPtr = dynamic_cast<const InMemIterator*>(&other);
                if (!otherPtr) return false;
                return it_ == otherPtr->it_;
            }

            bool operator!=(const DataIteratorBase& other) const override {
                return !(*this == other);
            }

            unique_ptr<DataIteratorBase> clone() const override {
                return unique_ptr<DataIteratorBase>(new InMemIterator(it_));
            }

            InnerIter get() const { return it_; }

        private:
            InnerIter it_;
    };

    /**
     * Specific in-memory constant iterator. 
     */
    class ConstInMemIterator : public DataIteratorBase {
        public:
            //Uses the iterator of a list internally
            using InnerIter = list<shared_ptr<DataObject>>::const_iterator;

            //Constructor
            explicit ConstInMemIterator(InnerIter it) : it_(it) {}

            //Pointer operations
            shared_ptr<DataObject> operator*() const override { return *it_; }
            DataIteratorBase& operator++() override { ++it_; return *this; }

            bool operator==(const DataIteratorBase& other) const override {
                auto o = dynamic_cast<const ConstInMemIterator*>(&other);
                return o && (it_ == o->it_);
            }

            bool operator!=(const DataIteratorBase& other) const override { return !(*this == other); }

            unique_ptr<DataIteratorBase> clone() const override {
                return unique_ptr<DataIteratorBase>(new ConstInMemIterator(it_));
            }

            InnerIter get() const { return it_; }

        private:
            InnerIter it_;
        };
    
    InMemContainer() = default;

    //Destructor clears and frees memory.
    ~InMemContainer(){clear();}
    
    //Iterator Methods
    iterator begin() override;
    iterator end() override;

    const_iterator begin() const override;
    const_iterator end() const override;
    
    //Parameter Getters
    size_type size() const override;
    bool empty() const override;
    
    //Manipulation
    void clear() override;
    void remove (const shared_ptr<DataObject>& val) override;
    void push_back (const shared_ptr<DataObject>& val) override;

    private:
    /** The container in which the data is stored in. */
    list<shared_ptr<DataObject>> myData;
};

#endif