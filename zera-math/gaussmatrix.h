#ifndef GAUSSMATRIX_H
#define GAUSSMATRIX_H

#include "zera-math_export.h"

/******************************************************************************************/
/**
 * @file gaussmatrix.h
 * @brief gaussmatrix.h holds all necessary declarations for the implemented gauss algorithm
 *
 * cGaussMatrix is used to compute a linear system of equations with variable order
 * @author Peter Lohmer p.lohmer@zera.de
*******************************************************************************************/

class cGaussMatrixPrivate; // forward
class cGaussNode;

class ZERA_MATH_EXPORT cGaussMatrix
{
public:
    /**
      @b Initialise the new matrix
      @param order defines the order of the new matrix
      */
    cGaussMatrix(int order);
    ~cGaussMatrix();
    /**
      @b Fill one row of the new matrix
      @param row defines which row to fill
      @param node gives the data point
      */
    void setMatrix(int row, cGaussNode node);
    /**
      @b Computes the matrix using elimination algorithm
      */
    void cmpKoeff();
    /**
      @b Reads out the computed coefficients
    */
    double getKoeff(int);
private:
    cGaussMatrixPrivate *d_ptr;
};


#endif // GAUSSMATRIX_H
