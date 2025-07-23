/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <GenSync/Data/DataObject.h>

/**
 * Implements a generic container that stores DataObject information. 
 * The manner in which data is stored is dependent on the subclass implementation.
 * DataContainers have their own iterator based off of DataIteratorBase allowing for looping and indexing.
 */
class DataContainer{
    protected:

        /**
         * Custom iterator built for all DataContainers.
         */
        class DataIterator {
            public:
            /**
             * Default Iterator Base Destructor
             */
            virtual ~DataIterator() = default;

            /**
             * Dereferences iterator to the DataObject pointer it refers to.
             * @return Returns the DataObject pointer the iterator is referencing.
             */
            virtual shared_ptr<DataObject> operator*() const = 0;

            /**
             * Moves the iterator up one element in the container's position.
             * Returns the pointer after it has moved position.
             * @return A pointer after the it has moved one position in the container.
             */
            virtual DataIterator& operator++() = 0;

            /**
             * Compares two DataIterators
             * @param other The other iterator.
             * @return Whether the two DataIterator point to the same object in the DataContainer.
             */
            virtual bool operator==(const DataIterator& other) const = 0;

            /**
             * Compares two DataIterator, returns true if they do not point to the same object in the DataContainer.
             * @param other The other iterator.
             * @return Whether the two DataIterator do not point to the same object in the DataContainer.
             */
            virtual bool operator!=(const DataIterator& other) const = 0;

            /**
             * Returns a pointer to a new iterator that references the same point.
             * @return A copy of a given iterator.
             */
            virtual unique_ptr<DataIterator> clone() const = 0;
        };

        /**
         * Wrapper for DataContainer iterator operations.
         * Allows DataIteratorBase implementations to be called until a general class.
         */
        class DataIteratorWrapper {
            public:
            //aliases
            using difference_type = ptrdiff_t;
            using value_type = shared_ptr<DataObject>;
            using pointer = shared_ptr<DataObject>*;
            using reference = shared_ptr<DataObject>&;
            using iterator_category = input_iterator_tag;

            /**
             * Constructs a new DataIteratorWrapper by moving the reference from the given pointer.
             * @param impl The original pointer.
             */
            DataIteratorWrapper(unique_ptr<DataContainer::DataIterator> impl) : _impl(std::move(impl)) {}

            /**
             * Constructs a new DataIteratorWrapper by cloning another Dataiterator's pointer.
             * @param other The DataIteratorWrapper to copy.
             */
            DataIteratorWrapper(const DataIteratorWrapper& other) : _impl(other._impl ? other._impl->clone() : nullptr) {}

            /**
             * Replaces the current pointer as the copy of another DataIterator's pointer.
             * @param other The DataIterator to copy.
             * @return The new copied iterator.
             */
            DataIteratorWrapper& operator=(const DataIteratorWrapper& other) {
                if (&other != this) {
                    _impl = other._impl ? other._impl->clone() : nullptr;
                }
                return *this;
            }

            /**
             * Moves the iterator up one element in the container's position.
             * Returns the iterator before its position is changed. 
             * @return The iterator before it moves position.
             */
            DataIteratorWrapper operator++(int) {
                DataIteratorWrapper temp(*this); 
                ++(*this);              
                return temp;           
            }
            
            /**
             * @return The DataObject pointer the iterator is pointing at.
             */
            shared_ptr<DataObject> operator*() const { return **_impl; }

            /**
             * Moves the iterator up one element in the container's position.
             * Returns the iterator after it moves position in the container. 
             * @return The iterator after it moves position in the container.
             */
            DataIteratorWrapper& operator++() { ++(*_impl); return *this; }

            /**
             * Compares two DataIteratorWrappers. 
             * Returns true if they point to the same object.
             * If the iterators have different DataContainer types they refer to, 
             * returns false by default as they are not able to point towards the same object.
             * @return Whether the iterators point to the same object in the container. If the the pointer types are different, returns false by default.
             */
            bool operator==(const DataIteratorWrapper& other) const {
                if (!_impl || !other._impl) return _impl == other._impl;
                return *_impl == *other._impl;
            }

            /**
             *  Compares two DataIteratorWrappers.
             *  Returns true if they do not point to the same object.
             *  If the iterators have different DataContainer types they refer to, 
             *  returns true by default as they are not able to point towards the same object.
             *  @return Whether the iterators do not point to the same object in the container. If the the pointer types are different, returns true by default.
             */
            bool operator!=(const DataIteratorWrapper& other) const { return !(*this == other); }

            private:
            /**
             * The implementation of the pointer.
             */
            unique_ptr<DataIterator> _impl;
        };

    public:
        //aliases
        using iterator = DataIteratorWrapper;
        using const_iterator = DataIteratorWrapper;
        using size_type = list<shared_ptr<DataObject>>::size_type;

        /**
         * Default destructor, meant to be overriden by subclasses.
         */
        virtual ~DataContainer() = default;

        /**
         * @return An iterator that points to the beginning of the container.
         */
        virtual iterator begin() = 0;

        /**
         * @return An iterator that points past the final item of the container.
         */
        virtual iterator end() = 0;

        /**
         * @return A const iterator that points to the beginning of the container.
         */
        virtual const_iterator begin() const = 0;

        /**
         * @return A const iterator that points to the beginning of the container.
         */
        virtual const_iterator end() const = 0;

        /**
         * @return The number of items inside the container.
         */
        virtual size_type size() const = 0;

        /**
         * @return Whether the container has no DataObjects.
         */
        virtual bool empty() const = 0;

        /**
         * Removes all DataObjects from the container.
         */
        virtual void clear() = 0;

        /**
         * Removes the first DataObject that contain the internal data as the given DataObject.
         * The internal data refers to the information that the DataObject represents.
         * @param val The given DataObject.
         * @return Returns true if object is successfully removed.
         */
        virtual bool remove(const shared_ptr<DataObject>& val) = 0;

        /**
         * Pushes a given DataObject into the container for storage.
         * @param val The given DataObject to store.
         */
        virtual void add(const shared_ptr<DataObject>& val) = 0;
};
#endif