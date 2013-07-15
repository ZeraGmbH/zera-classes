#include "gaussmatrix.h"
#include "gaussnode.h"
#include "gaussmatrix_p.h"

cGaussMatrix::cGaussMatrix(int order)
    :d_ptr(new cGaussMatrixPrivate(order))
{
}


cGaussMatrix::~cGaussMatrix()
{
    delete d_ptr;
}


void cGaussMatrix::setMatrix(int row, cGaussNode node)
{
    d_ptr->setMatrix(row, node);
}


void cGaussMatrix::cmpKoeff()  // computing the coefficients by elimination algorithm
{
    d_ptr->cmpKoeff();
}


double cGaussMatrix::getKoeff(int n)
{
    return d_ptr->getKoeff(n);
}

