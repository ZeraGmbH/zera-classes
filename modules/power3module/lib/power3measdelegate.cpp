#include "power3measdelegate.h"
#include "vfmodulecomponent.h"
#include <cmath>

namespace POWER3MODULE
{

cPower3MeasDelegate::cPower3MeasDelegate(VfModuleComponent *pactvalue, VfModuleComponent *qactvalue, VfModuleComponent *sactvalue, bool withSignal)
    :m_ppActValue(pactvalue), m_pqActValue(qactvalue), m_psActValue(sactvalue), m_bSignal(withSignal)
{
}


void cPower3MeasDelegate::actValueInputU(QVariant val)
{
    input1 = val.value<QList<double> >();
    computeOutput();
}


void cPower3MeasDelegate::actValueInputI(QVariant val)
{
    input2 = val.value<QList<double> >();
    if (m_bSignal)
        emit measuring(0);
    computeOutput();
    if (m_bSignal)
        emit measuring(1);
}


void cPower3MeasDelegate::computeOutput()
{
    QList<double> resultListP, resultListQ, resultListS;

    int n = input1.count() < input2.count() ? input1.count() : input2.count();
    n = n >> 1; // we always compute pairs of values
    if (n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            double real1, im1, real2, im2, resultReal, resultIm;
            // the fft algorithm provides information for sine
            // for power we are interested in cos .... so we change re and im
            //real1 = input1.at(i*2+1); im1 = input1.at(i*2);
            //real2 = input2.at(i*2+1); im2 = input2.at(i*2);

            real1 = input1.at(i*2); im1 = input1.at(i*2+1);
            real2 = input2.at(i*2); im2 = input2.at(i*2+1);

            // additionally we have to devide by 2.0 because we get the amplitude information
            // rather then energy information
            resultReal = ((real1 * real2) + (im1 * im2)) / 2.0;
            resultIm = ((real1 * im2) - (real2 *im1)) / 2.0;

            resultListP.append(resultReal);
            resultListQ.append(resultIm);
            resultListS.append(sqrt((resultReal * resultReal) + (resultIm * resultIm)));
        }

        QVariant list;
        list = QVariant::fromValue<QList<double> >(resultListP);
        m_ppActValue->setValue(list);
        list = QVariant::fromValue<QList<double> >(resultListQ);
        m_pqActValue->setValue(list);
        list = QVariant::fromValue<QList<double> >(resultListS);
        m_psActValue->setValue(list);

    }

}


}
