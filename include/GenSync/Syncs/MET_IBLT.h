/* This code is part of the GenSync project developed at Boston University.  Please see the README for use and references. */

/*
 * Multi-Edge-Type Invertible Bloom Lookup Table
 * 
 * The MET-IBLT is a novel IBLT-based set reconciliation protocol that does not
 * require estimation of the size of the set-difference. This is due to the
 * scalable nature of the MET-IBLT data structure.
 * 
 * Citation for the original paper outlining MET IBLTs:
 * F. Lazaro and B. Matuz, "A Rate-Compatible Solution to the Set Reconciliation Problem,"
 * in IEEE Transactions on Communications, vol. 71, no. 10, pp 5769-5782, Oct. 2023.
 * 
 * Code was inspired by Python implementation of MET IBLT:
 * github.com/avi1mizrahi/MET_IBF
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

    // Default Constructor
    MET_IBLT();

    // Default Destructor
    ~MET_IBLT();

    /**
     * Constructs a MET IBLT.
     * @param deg_matrix The degree matrix specifying the number of hashes between each cell and element type.
     * @param m_cells The list of cell types, each index specifies type, and value at index specifies number of cells.
     * @param key2type Function which returns element type given an element (key).
     * @param eltSize Size of elements being stored.
     */
    MET_IBLT(const vector<vector<int>>& deg_matrix, 
             const vector<int>& m_cells, 
             function<int(ZZ)>& key2type, size_t eltSize);

    /**
     * Getter method for the MET IBLT's Degree Matrix.
     * @return vector<vector<int>> The deg_matrix used by the MET IBLT.
     */
    vector<vector<int>> getDegMatrix();

    /**
     * Getter method for the MET IBLT's Cell Types.
     * @return vector<vector<int>> The m_cells or list of cell types used by the MET IBLT.
     */
    vector<int> getCellTypes();

    /**
     * Getter method for a table of a specific type in MET IBLT.
     * @param cellType The cell type (index) of the desired table (GenIBLT).
     * @return GenIBLT The GenIBLT representing the table/cells of cellType.
     */
    GenIBLT getTable(int cellType);

    /**
     * Insert an element into MET IBLT.
     * @param value The element to be added to MET IBLT.
     */
    void insert(ZZ value);

    /**
     * Insert an element into MET IBLT to a specific cell type.
     * @param value The element to be added to MET IBLT.
     * @param cellType The cell type the element will be added to.
     */
    void insert(ZZ value, int cellType);

    /**
     * Erases an element from the MET IBLT.
     * This operation always succeeds.
     * @param value The value to be removed.
     */
    void erase(ZZ value);

    /**
     * TODO: Iterates through all tables ONCE and attempts to recover each.
     * @param positive The resulting list of elements peeled with positive count in MET IBLT.
     * @param negative The resulting list of elements peeled with negative count in MET IBLT.
     * @return true iff at least one of the tables were completely peeled/recovered.
     */
    bool peelOnce(std::set<ZZ> &positive, std::set<ZZ> &negative);

    /**
     * TODO: Attempts to recover all elements from MET IBLT by repeatedly performing peel operations.
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

    /**
     * Adds new cell type to MET IBLT.
     * @warning This does not automatically insert existing elements into the new cell type.
     * @param size Number of cells in cell type.
     * @param elemHashes Vector specifying element type hashes. To be added to degree matrix.
     */
    void addCellType(int size, vector<int> elemHashes);

    /**
     * Adds GenIBLT to end of MET IBLT's list of tables.
     * @warning This should ONLY be used when performing peelAll on a difference MET IBLT.
     * @warning This may make the MET IBLT INCONSISTENT if used in any other situation.
     * @param newIBLT GenIBLT to be added.
     */
    void addGenIBLT(GenIBLT newIBLT);

private:

    /**
     * Creates hasher function for a GenIBLT in tables.
     * @param cellType Index of GenIBLT in tables or cell type.
     * @return A hasher function or which returns number of hashes for an element according to type and degree matrix.
     */
    function<long(ZZ)> createHasher(int cellType);

    // Degree matrix. Cell Types X Elem Types
    vector<vector<int>> deg_matrix;

    // Cell Types, index is cell type, value is number of cells
    vector<int> m_cells;

    // Returns Element's Type given Element
    function<int(ZZ)> key2type;

    // List of GenIBLTs according to Degree Matrix and Cells
    vector<GenIBLT> tables;

    // Size of elements being stored
    size_t eltSize;
};

#endif