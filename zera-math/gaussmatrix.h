#ifndef GAUSSMATRIX_H
#define GAUSSMATRIX_H

#include "math_global.h"

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

/**
  @b class for a gauss matrix
  */
class ZERA_MATHSHARED_EXPORT cGaussMatrix
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
    /**
      @b D'pointer to the private library internal structure

      this is used to hide the internal structure, and thus make the library ABI safe
      */
    cGaussMatrixPrivate *d_ptr;
};


#endif // GAUSSMATRIX_H
