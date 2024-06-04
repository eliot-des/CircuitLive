/*
  ==============================================================================

    eigenWrapper.h
    Created: 28 May 2024 10:07:27am
    Author:  eliot

  ==============================================================================
*/

#pragma once
#include <Eigen/Sparse>

// Wrapper class to provide easy access to non-zero entries or a shared dummy value
class SparseMatrixEntry {
public:
    SparseMatrixEntry() : valueRef_(&dummyValue_) {}  // Default constructor

    // Initialize the object with a sparse matrix and indices
    void init(Eigen::SparseMatrix<double>& matrix, int row, int col) {
        matrix_ = &matrix;
        row_ = row;
        col_ = col;

        //If the indices are zero, it means that the components is connected to the ground.
        //Since the ground is not a part of the matrix, we set the valueRef_ to the static 
        //dummyValue_ if on of the entry position (row, col) is a zero.

        if (row == 0 || col == 0) {
            valueRef_ = &dummyValue_;
            return;
        }

        //The first row and column of the matrix therefore refers to the node 1 and we
        //need to subtract 1 from the row and column to get the correct index in the matrix
        row_--;
        col_--;

        //Search for the non-zero entry -> if it exists, set valueRef_ to the entry value
        for (int k = 0; k < matrix_->outerSize(); ++k) {
            for (Eigen::SparseMatrix<double>::InnerIterator it(*matrix_, k); it; ++it) {
                if (it.row() == row_ && it.col() == col_) {
                    valueRef_ = &it.valueRef();
                    return;
                }
            }
        }
        //If the entry is zero, set valueRef_ to the static dummyValue_
        valueRef_ = &dummyValue_;
    }

    SparseMatrixEntry& operator+=(double val) {
        *valueRef_ += val;
        return *this;
    }

    SparseMatrixEntry& operator-=(double val) {
        *valueRef_ -= val;
        return *this;
    }

    SparseMatrixEntry& operator=(double val) {
        *valueRef_ = val;
        return *this;
    }

    // Implicit conversion to double to easily read the value
    operator double() const {
        return *valueRef_;
    }

private:
    double* valueRef_;
    static double dummyValue_;  // Static member to hold the dummy value
    Eigen::SparseMatrix<double>* matrix_ = nullptr;
    int row_ = 0;
    int col_ = 0;
};


// Wrapper class to provide easy access to vector entries or a shared zero value
class VectorEntry {
public:
    VectorEntry() : valueRef_(&zeroValue_) {}  // Default constructor

    // Initialize the object with a vector and an index
    void init(Eigen::VectorXd& vector, int idx) {
        vector_ = &vector;
        idx_ = idx;

        //If the index is zero, it means that the component is connected to the ground.
        //Since the ground is not a part of the vector, we set the valueRef_ to the static
        //zeroValue_ if the index is zero.

        //If the index is not zero, we set the valueRef_ to the entry value in the vector.
        //The first entry in the vector refers to the node 1 and we need to subtract 1 from
        //the index to get the correct index in the vector (index 0 is the node 1, index 1
        // is node 2 and so on).

        if (idx == 0) {
            valueRef_ = &zeroValue_;
        }
        else {
            valueRef_ = &(*vector_)(idx - 1);
        }
    }

    // Operator to easily modify the value
    VectorEntry& operator+=(double val) {
        *valueRef_ += val;
        return *this;
    }

    VectorEntry& operator-=(double val) {
        *valueRef_ -= val;
        return *this;
    }

    VectorEntry& operator=(double val) {
        *valueRef_ = val;
        return *this;
    }

    // Implicit conversion to double to easily read the value
    operator double() const {
        return *valueRef_;
    }

private:
    double* valueRef_;
    double zeroValue_ = 0.0;  // Static member to hold the zero value
    Eigen::VectorXd* vector_ = nullptr;
    int idx_ = 0;
};

