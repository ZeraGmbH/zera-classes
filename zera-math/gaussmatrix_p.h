#ifndef GAUSSMATRIX_P_H
#define GAUSSMATRIX_P_H


/**
  @b class for a data point
  */
struct cGaussNode {
    /**
      @b the data point's value
      */
    double m_fNode;
    /**
      @b the data point's argument
      */
    double m_fArg;
};


/**
  @brief
  The implemention for cGaussMatrix.
  */
class cGaussMatrixPrivate {
public:
    /**
      @b Initialise the new matrix
      @param order defines the order of the new matrix
      */
    cGaussMatrixPrivate(int order);
    ~cGaussMatrixPrivate();
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
    int m_norder; // order
    double *m_py; // order * data value
    double *m_pkoeff; // order * koefficients
    double *m_pmatrix; // order * order arguments (sqare matrix)
};



#endif // GAUSSMATRIX_P_H
