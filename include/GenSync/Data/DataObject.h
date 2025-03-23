/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

#ifndef DATA_OBJECT_H
#define DATA_OBJECT_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <map>
#include <list>

#include <GenSync/Auxiliary/UID.h>
#include <GenSync/Auxiliary/Auxiliary.h>

// namespaces
using std::string;
using std::ostream;

/**
 *  DataObject.h -- a wrapper for a generic set element to be synchronized
 */
class DataObject : public UID {
public:
    // Constructors

    /**
     * Construct an empty data object.
     */
    DataObject();

    /**
     * Constructs a data object that contains the given byte vector (not a copy!).
     * @param data An vector of bytes containing the data of the object.
     */
    explicit DataObject(const ZZ &datum);

    /**
     * Constructs a data object that contains the given string (in an encoded format).
     * @param data The string to place in the DataObject.
     */
    explicit DataObject(const string& str);
    
     /**
     * Constructs a data object that contains a given pair {number, list of elements}
     * @param index a number in a templated type
     * @param elems a list containing pointers to data object
     **/
    template <class T>
    explicit DataObject(const T index, const list<shared_ptr<DataObject>>& elems)
    {
        string str = base64_encode(toStr<T>(index).c_str(), toStr<T>(index).length()) + " ";
        for (const auto& itr : elems)
            str += base64_encode(itr->to_string().c_str(), itr->to_string().length()) + " ";
        str = base64_encode(str.c_str(),str.length());
        myBuffer = pack(str);
    }

    /**
     * Constructs a data object that contains a given set
     * @param mySet The set to place in the DataObject
     **/
    explicit DataObject(const multiset<shared_ptr<DataObject>>& mySet);

    /**
     * Constructs a data object that contains the given object of type T, which must
     * be translatable to a string with the global toStr function.  In effect, this constructor
     * simply converts the object to a string and then does the same as the string constructor.
     * @param item The item to place in the DataObject.
     */
    template<typename T>
    explicit DataObject(const T item) {
        myBuffer = pack(toStr(item));
    }
    
    // INFORMATION METHODS


    // MANIPULATION METHODS
    
    /**
     * @return A ZZ encoding of the string
     */
    ZZ to_ZZ() const;
    
    /**
     * @return The string version of a copy of the contents this data object.
     *         The string could have null bytes and non-printable characters.
     */
    string to_string() const;

    /**
    * @return a multiset containing all the data object in the set
    **/
    multiset<shared_ptr<DataObject>> to_Set() const;

    /**
     * @param ind no specific usage but to indicate class type T for the first 
     *        element in the element pair
     * @return a pair {long ,list<shared_ptr<DataObject>>}
     **/
    template <class T>
    pair<T, list<shared_ptr<DataObject>>> to_pair(){
        
        string str = unpack(myBuffer);
        auto splt = split(base64_decode(str), ' ');

        T out = strTo<T>(base64_decode(splt[0]));
        list<shared_ptr<DataObject>> outList;
        for (int ii = 1; ii < splt.size(); ii++)
            outList.push_back(make_shared<DataObject>(base64_decode(splt[ii])));
        return {out, outList};
    }


    /**
     * @param len Stores the length of the char array (upon return)
     * @return A char array version of the contents of this data object.
     *         The string could have null bytes and non-printable characters.
     */
    const char *to_char_array(size_t &len) const;

    /**
     * @return A string of printable characters representing the object, essentially
     * a base64 encoding of the internal contents of the DataObject.
     */
    string print() const;

    /**
     *    Allows stream printing of the object
     */
    friend ostream& operator<<(ostream& out, const DataObject &datum);

    // comparisons
    bool operator < (const DataObject& second) const{
        return (this->myBuffer < second.to_ZZ());
    }

    bool operator== (const DataObject& second) const {
        return (this->myBuffer == second.to_ZZ());
    }
       
    // static variables
    static bool RepIsInt; /** if true, then DataObject string inputs are interpreted as
                           * arbitrarily-sized integers.  Otherwise, DataObject string inputs
                           * are interpreted as byte sequences.
                           */
    void setTimeStamp(clock_t ts);
    clock_t getTimeStamp();
protected:
    ZZ myBuffer; /** The buffer for the data object container itself. */
    clock_t timestamp=clock();;

    /**
  * Unpacks a ZZ into a string
  * @param num
  * @return
  */
    static string unpack(const ZZ& num);

private:
    /**
     * Packs a string into a ZZ in a memory-efficient manner
     * @param theStr The string to pack
     * @return a ZZ representing the string
     */
    static ZZ pack(const string& theStr);

};
#endif
