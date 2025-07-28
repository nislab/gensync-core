/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */
// Created by GregoryFan on 7/10/2025
//

#ifdef USE_SQLITE
#ifndef SQLITE_CONTAINER_H
#define SQLITE_CONTAINER_H

#include <GenSync/Data/DatabaseContainer.h>
#include <sqlite3.h>

class SQLiteContainer : public DatabaseContainer {
    protected:
    /**
     * SQLite-based iterator for standard operation on an SQLite Table.
     */
    class SQLiteIterator : public DataIterator {
        public:
        /**
         * Constructs an iterator using a given database with
         * a query on where to go.
         */
        SQLiteIterator(sqlite3* db, const string& query);

        /**
         * Constructs a default iterator with no given database. 
         */
        SQLiteIterator();
        
        /**
         * Default destructor that finalizes and closes its statement.
         */
        ~SQLiteIterator();

        /**
         * @return A DataObject pointer that is reconstructed from the data the iterator is pointing at.
         */
        shared_ptr<DataObject> operator*() const override;

        /**
         * Moves the iterator up in the container's memory.
         * Returns the iterator after it has moved positions.
         * @return The iterator after it has moved positions in the container.
         */
        DataIterator& operator++() override;

        /**
         * Compares two iterators.
         * Returns true if they fulfill three conditions:
         * the other iterator is of type SQLiteContainer,
         * both iterators have statements that point to the same database and table,
         * and that both iterators are on the same row.
         * @return Whether the two iterators point to the same information in a database and table.
         */
        bool operator==(const DataIterator& other) const override;
        
         /**
         * Compares two iterators.
         * Returns false if they fulfill three conditions:
         * the other iterator is of type SQLiteContainer,
         * both iterators have statements that point to the same database and table,
         * and that both iterators are on the same row.
         * @return Whether the two iterators do not point to the same information in a database and table.
         */
        bool operator!=(const DataIterator& other) const override;
        
        /**
         * Creates a new iterator that points to the same object in the container.
         * The iterator is destroyed when the pointer referring to it is moved or destroyed.
         * @return A new iterator that is identical to this one.
         */
        unique_ptr<DataIterator> clone() const override;

        private:
        /**
         * The SQLite database the iterator refers to.
         */
        sqlite3* _db;

        /**
         * The SQLite statement the iterator is following.
         */
        sqlite3_stmt* _stmt;

        /**
         * The SQLite query that the iterator transforms into a statement.
         */
        string _query;

        /**
         * The index of the current row the iterator is at.
         */
        int _currentRow;

        /**
         * Whether the iterator is done with its statement.
         */
        bool _done;
        
        /**
         * Moves the iterator up in the table, if possible.
         */
        void step();
    };

    public:
    /**
     * Constructs a SQLiteContainer with a given database reference and table name.
     * The database and table are created if they do not exist.
     * @param ref The reference to the database by name.
     * @param tableName The name of the table.
     */
    SQLiteContainer(const string& ref, const string& tableName = "defaultTable");

    /**
     * Default Constructor. Closes the database if it exists.
     */
    ~SQLiteContainer();

    /**
     * @return An iterator that points to the beginning of the container.
     */
    iterator begin() override;

    /**
     * @return An iterator that has finished and has no statement running.
     * This iterator is meant the mark the end state of an iterator.
     */
    iterator end() override;

    /**
     * @return An const iterator that points to the beginning of the container.
     */
    const_iterator begin() const override;

    /**
     * @return A const iterator that has finished and has no statement running.
     * This iterator is meant the mark the end state of an iterator.
     */
    const_iterator end() const override;

    /**
     * @return The number of items inside the table.
     */
    size_type size() const override;

    /**
     * @return Whether the table has no items.
     */
    bool empty() const override;

    /**
     * Removes all items from the table.
     */
    void clear() override;

    /**
     * Removes the first entry that contain the internal data as the given DataObject.
     * The internal data refers to the information that the DataObject represents.
     * @param val The given DataObject.
     * @return Returns true if object is successfully removed.
     */
    bool remove(const shared_ptr<DataObject>& val) override;

    /**
     * Adds an entry with the internal data as the given DataObject.
     * The internal data refers to the information that the DataObject represents.
     * @param val The given DataObject.
     */
    void add(const shared_ptr<DataObject>& val) override;

    private:
    /** The table name of the SQLite table the container will read and write to.*/
    string tableName;

    /** The pointer to the SQLite database.*/
    sqlite3* db;
};
#endif //SQLITE_CONTAINER_H
#endif