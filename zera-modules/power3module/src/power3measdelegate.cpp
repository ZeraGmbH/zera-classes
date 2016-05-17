#include <QDebug>

#include <veinmoduleactvalue.h>

#include "debug.h"
#include "power3measdelegate.h"


namespace  POWER3MODULE
{

cPower3MeasDelegate::cPower3MeasDelegate(cVeinModuleActvalue *actvalue)
    :m_pActValue(actvalue)
{
}


void cPower3MeasDelegate::actValueInput1(QVariant val)
{
    input1 = val.value<QList<double> >();
    computeOutput();
}


void cPower3MeasDelegate::actValueInput2(QVariant val)
{
    input2 = val.value<QList<double> >();
    computeOutput();
}


void cPower3MeasDelegate::computeOutput()
{
    QList<double> resultList;

    int n = input1.count() < input2.count() ? input1.count() : input2.count();
    n = n >> 1; // we always compute pairs of values
    if (n > 0)
    {
        for (int i = 0; i < n; i++)
        {
            double real1, im1, real2, im2, resultReal, resultIm;
            // the fft algorithm provides information for sine
            // for power we are interested in cos .... so we change re and im
            real1 = input1.at(i*2+1); im1 = input1.at(i*2);
            real2 = input2.at(i*2+1); im2 = input2.at(i*2);

#ifdef DEBUG
            if (i==1)
            {
                QString ts;
                ts = QString("Real1: %1 ; Imag1: %2 ; Real2: %3 ; Imag2: %4").arg(real1).arg(im1).arg(real2).arg(im2);
                qDebug() << ts;
            }
#endif

            // additionally we have to devide by 2.0 because we get the amplitude information
            // rather then energy information
            resultReal = ((real1 * real2) + (im1 * im2)) / 2.0;
            resultIm = -((real1 * im2) - (real2 *im1)) / 2.0;

            resultList.append(resultReal);
            resultList.append(resultIm);
        }

        QVariant list;
        list = QVariant::fromValue<QList<double> >(resultList);
        m_pActValue->setValue(list);

#ifdef DEBUG
        QString ts;
        for (int j = 0; j < (resultList.count() >> 1); j++)
            ts= ts + QString("%1,%2;").arg(resultList.at(j*2)).arg(resultList.at(j*2+1));
        qDebug() << ts;
#endif
    }

}


}
