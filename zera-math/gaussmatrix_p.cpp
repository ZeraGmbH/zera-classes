#include "gaussmatrix_p.h"
#include "gaussnode.h"

cGaussMatrixPrivate::cGaussMatrixPrivate(int order)
{
    m_norder = order;
    m_py = new double[order];
    m_pkoeff = new double[order];
    m_pmatrix = new double[order*order];
}


cGaussMatrixPrivate::~cGaussMatrixPrivate() {
    delete[] m_py;
    delete[] m_pkoeff;
    delete[] m_pmatrix;
}


void cGaussMatrixPrivate::setMatrix(int row, cGaussNode node)
{
    m_py[row] = node.m_fNode; // yz
    int i, aoffs;
    double x = node.m_fArg; // start value
    double ko = 1.0;
    aoffs = (row + 1) * m_norder -1; // adress offset
    for (i = 0; i < m_norder; i++) // yz = az*x^3 +bz*x^2 +cz*x^1 +dz*x^0
    {
        m_pmatrix[aoffs - i] = ko;
        ko *= x; // exponentiate
    }
    // one row in matrix initialized
}


void cGaussMatrixPrivate::cmpKoeff()  // computing the coefficients by elimination algorithm
{
    int i,row,column;
    double fak;
    for (i = 1; i < m_norder; i++)
    {
        for (row = i; row < m_norder; row++)
        {
            if (m_pmatrix[row * m_norder + i - 1] != 0.0)
                fak = -m_pmatrix[(i-1) *  m_norder + i - 1] / m_pmatrix[row * m_norder + i - 1];
            else
                fak = 0.0;
            m_py[row] = fak * m_py[row] + m_py[i-1];
            for (column = i-1; column < m_norder; column++)
                m_pmatrix[row * m_norder + column] = fak * m_pmatrix[row * m_norder  + column] + m_pmatrix[(i-1) * m_norder +column];
        }
    } // gauss eliminiation done

    for (i = 0; i < m_norder; i++) // calculating the coefficients now
    {
        row = m_norder -1 - i; // ew begin in the last row
        m_pkoeff[row] = m_py[row];
        if (i > 0) // and move upward
        {
            for (column = row+1; column < m_norder; column++)
                m_pkoeff[row] -= m_pmatrix[row * m_norder + column] * m_pkoeff[column];
        }
        if (m_pmatrix[row * m_norder + row] != 0.0)
            m_pkoeff[row] /= m_pmatrix[row * m_norder + row];
        else
            m_pkoeff[row] = 0.0;
    }
}


double cGaussMatrixPrivate::getKoeff(int n)
{
    return m_pkoeff[n];
}
