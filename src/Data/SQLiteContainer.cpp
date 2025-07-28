//
// Created by GregoryFan on 7/10/2025
//

#ifdef USE_SQLITE
#include <GenSync/Data/SQLiteContainer.h>

//SQLiteIterator Functions

SQLiteContainer::SQLiteIterator::SQLiteIterator(sqlite3* db, const string& query): _query(query), _currentRow(0), _db(db), _done(true), _stmt(nullptr){
    //Prepares the Iterator with a given database and query.
    if (sqlite3_prepare_v2(_db, query.c_str(), -1, &_stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement");
    }
    //Enters the first element.
    step();
}

SQLiteContainer::SQLiteIterator::SQLiteIterator(): _db(nullptr), _stmt(nullptr), _done(true){}

SQLiteContainer::SQLiteIterator::~SQLiteIterator() {
    if (_stmt) sqlite3_finalize(_stmt);
}

shared_ptr<DataObject> SQLiteContainer::SQLiteIterator::operator*() const{
    //Trying to dereference end iterator
    if (_done) throw std::runtime_error("Dereferencing end iterator");
    
    //Extracts data and creates DataObject from it.
    const void* blob = sqlite3_column_blob(_stmt, 1);
    int size = sqlite3_column_bytes(_stmt, 1);
    string data(reinterpret_cast<const char*>(blob), size);
    return make_shared<DataObject>(data);
}

SQLiteContainer::DataIterator& SQLiteContainer::SQLiteIterator::operator++(){
    step();
    return *this;
}

bool SQLiteContainer::SQLiteIterator::operator==(const DataIterator& other) const{
    //Ensures other is a SQLiteIterator
    auto o = dynamic_cast<const SQLiteIterator*>(&other);
    if (!o) return false;

    //Compares by doneness or by having the same row and statement.
    return _done == o->_done || (_stmt == o->_stmt && _currentRow == o->_currentRow);
}

bool SQLiteContainer::SQLiteIterator::operator!=(const DataIterator& other) const{
    return !(*this == other);
}

unique_ptr<SQLiteContainer::DataIterator> SQLiteContainer::SQLiteIterator::clone() const{
    auto cloned = unique_ptr<SQLiteIterator>(new SQLiteIterator(_db, _query));

    // Step to same position
    for (int i = 1; i < _currentRow; ++i) {
        cloned->step();
    }

    return cloned;
}

void SQLiteContainer::SQLiteIterator::step(){
    //Statement is not set up.
    if (!_stmt) return; 

    //Goes onto the next item in the table.
    int rc = sqlite3_step(_stmt);
    if (rc == SQLITE_ROW) {
        //More elements to go through
        _done = false;
        ++_currentRow;
    } else if (rc == SQLITE_DONE) {
        //No more elements to go through
        _done = true;
        sqlite3_finalize(_stmt);
        _stmt = nullptr;
    } else {
        throw std::runtime_error("SQLite step error");
    }
}


//SQLiteContainer Functions

SQLiteContainer::SQLiteContainer(const string& ref, const string& table) : tableName(table) {
    //Opens the database if possible.
    if (sqlite3_open(ref.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
        return;
    }

    //Creates a table if needed.
    string statement = 
        "CREATE TABLE IF NOT EXISTS " + tableName + " ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "data BLOB);";
    
    char* errMsg = nullptr;
    if (sqlite3_exec(db, statement.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to create table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        db = nullptr;
    }
}

SQLiteContainer::~SQLiteContainer(){
    if (db) {
        sqlite3_close(db);
    }
}

//Methods
SQLiteContainer::iterator SQLiteContainer::begin(){
    //Takes every item from the list for the iterator to go through.
    string query = "SELECT * FROM " + tableName;
    return unique_ptr<DataIterator>(new SQLiteIterator(db, query));
}

SQLiteContainer::iterator SQLiteContainer::end(){
    //Sends a blank iterator to denote the end of a iterator statement.
    return unique_ptr<DataIterator>(new SQLiteIterator());
}

SQLiteContainer::const_iterator SQLiteContainer::begin() const{
    //Takes every item from the list for the iterator to go through.
    string query = "SELECT * FROM " + tableName;
    return unique_ptr<DataIterator>(new SQLiteIterator(db, query));
}

SQLiteContainer::const_iterator SQLiteContainer::end() const{
    //Sends a blank iterator to denote the end of a iterator statement.
     return unique_ptr<DataIterator>(new SQLiteIterator());
}

DataContainer::size_type SQLiteContainer::size() const{
    sqlite3_stmt* stmt = nullptr;
    string query = "SELECT COUNT(*) FROM " + tableName;
    DataContainer::size_type count = 0;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare COUNT query: " + std::string(sqlite3_errmsg(db)));
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = static_cast<DataContainer::size_type>(sqlite3_column_int(stmt, 0));
    } else {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute COUNT query: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
    return count;
}

bool SQLiteContainer::empty() const{
    //Takes a single element to check if it is empty.
    sqlite3_stmt* stmt;
    string sql = "SELECT 1 FROM " + tableName + " LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare empty check: " + std::string(sqlite3_errmsg(db)));
    }

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc != SQLITE_ROW; 
}

void SQLiteContainer::clear(){
    sqlite3_stmt* stmt;
    string sql = "DELETE FROM " + tableName + ";";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare clear: " + std::string(sqlite3_errmsg(db)));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to clear table: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
}

bool SQLiteContainer::remove(const shared_ptr<DataObject>& val){
    sqlite3_stmt* stmt;
    int sizeBefore = size();
    string data = val->to_string();
    string statement = "DELETE FROM " + tableName + " WHERE data=? LIMIT 1";
    
    if (sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }

    if (sqlite3_bind_blob(stmt, 1, val->to_string().data(), static_cast<int>(val->to_string().size()), SQLITE_TRANSIENT) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind id");
    }
 
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute delete");
    }

    sqlite3_finalize(stmt);

    return sizeBefore < size();
}

void SQLiteContainer::add(const shared_ptr<DataObject>& val){
    sqlite3_stmt* stmt;
    string statement = "INSERT INTO " + tableName + " (data) VALUES (?);";

    if (sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)));
    }

    if (sqlite3_bind_blob(stmt, 1, val->to_string().data(), static_cast<int>(val->to_string().size()), SQLITE_TRANSIENT) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to bind data");
    }

    // Execute the statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute insert" + err);
    }

    // Get the auto-generated ID
    int inserted_id = static_cast<int>(sqlite3_last_insert_rowid(db));
    val->setObjectID(inserted_id);

    // Finalize to free the statement object
    sqlite3_finalize(stmt);
}
#endif
