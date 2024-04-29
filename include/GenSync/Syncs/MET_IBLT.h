/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * Multi-Edge-Type Invertible Bloom Filter
 *
 * Created by Anish Sinha on 2/19/24.
 */

#ifndef GENSYNCLIB_METIBLT_H
#define GENSYNCLIB_METIBLT_H

#include <vector>
#include <string>
#include <functional>
#include <NTL/ZZ.h>
#include <GenSync/Syncs/GenIBLT.h>

using std::string;
using std::vector;
using namespace NTL;

class MET_IBLT
{
public:
    friend class MET_IBLTSync;

    // Default Constructor
    MET_IBLT();

    // default destructor
    ~MET_IBLT();

    /**
     * Constructs a MET IBLT.
     * @param deg_matrix The degree matrix specifying the number of hashes between each cell and element type.
     * @param m_cells The list of cell types, each index specifies type and value at index specifies number of cells.
     * @param key2type Function which returns element type given an element (key).
     * @param eltSize Size of elements being stored.
     */
    MET_IBLT(const vector<vector<int>>& deg_matrix, 
             const vector<int>& m_cells, 
             function<int(ZZ)> key2type, size_t eltSize);

    /**
     * Insert an element into MET IBLT.
     * @param value The element to be added to MET IBLT.
     */
    void insert(ZZ value);

    /**
     * Erases an element from the MET IBLT.
     * This operation always succeeds.
     * @param value The value to be removed.
     */
    void erase(ZZ value);

    /**
     * TODO: DOCUMENTATION NEEDED
     * @param positive The resulting list of elements peeled with positive count in MET IBLT.
     * @param negative The resulting list of elements peeled with negative count in MET IBLT.
     * @return true iff all elements successfully peeled.
     */
    bool peelOnce(std::set<ZZ> &positive, std::set<ZZ> &negative);

    /**
     * TODO: DOCUMENTATION NEEDED
     * @param positive The resulting list of elements peeled with positive count in MET IBLT.
     * @param negative The resulting list of elements peeled with negative count in MET IBLT.
     * @return true iff all elements successfully peeled.
     */
    bool peelAll(vector<ZZ> &positive, vector<ZZ> &negative);

    /**
     * Convert MET IBLT to a readable string.
     * @return A human-readable string describing the contents of the MET IBLT.
     */
    string toString();

private:
    /**
     * Insert an element into MET IBLT to a specific cell type.
     * @param value The element to be added to MET IBLT.
     * @param cellType The cell type the element will be added to.
     */
    void insert(ZZ value, int mIndex);

    /**
     * Add cell type to MET IBLT.
     * @param size Number of cells in cell type.
     * @param elemHashes Vector specifying element type hashes. To be added to degree matrix.
     */
    void addCellType(int size, vector<int> elemHashes);

    /**
     * Creates hasher function for an IBLT in tables.
     * @param cellType Index of IBLT in tables or cell type.
     * @return A hasher function or which returns number of hashes for an element according to type and degree matrix.
     */
    function<long(ZZ)> createHasher(int cellType);

    // Degree matrix. Cell Types X Elem Types
    vector<vector<int>> deg_matrix;

    // Cells, index is cell type, value is number of cells
    vector<int> m_cells;

    // Returns Element's Type given Element
    function<int(ZZ)> key2type;

    // List of IBLTs according to Degree Matrix and Cells
    vector<GenIBLT> tables;

    // Size of elements being stored
    size_t eltSize;
};

#endif