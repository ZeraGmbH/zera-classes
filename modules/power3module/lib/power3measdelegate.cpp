#include "power3measdelegate.h"
#include "vfmodulecomponent.h"
#include <QList>
#include <QVariant>
#include <cmath>

namespace POWER3MODULE
{

cPower3MeasDelegate::cPower3MeasDelegate(VeinStorage::AbstractComponentPtr fftU,
                                         VeinStorage::AbstractComponentPtr fftI,
                                         VfModuleComponent *pactvalue,
                                         VfModuleComponent *qactvalue,
                                         VfModuleComponent *sactvalue) :
    m_fftU(fftU),
    m_fftI(fftI),
    m_ppActValue(pactvalue),
    m_pqActValue(qactvalue),
    m_psActValue(sactvalue)
{
}

void cPower3MeasDelegate::computeOutput()
{
    QList<double> inputU = m_fftU->getValue().value<QList<double> >();
    QList<double> inputI = m_fftI->getValue().value<QList<double> >();

    int n = inputU.count() < inputI.count() ? inputU.count() : inputI.count();
    n = n >> 1; // we always compute pairs of values
    if (n > 0) {
        QList<double> resultListP, resultListQ, resultListS;
        for (int i = 0; i < n; i++) {
            double real1, im1, real2, im2, resultReal, resultIm;

            // the fft algorithm provides information for sine
            // for power we are interested in cos .... so we change re and im
            //real1 = inputU.at(i*2+1); im1 = inputU.at(i*2);
            //real2 = inputI.at(i*2+1); im2 = inputI.at(i*2);

            real1 = inputU.at(i*2); im1 = inputU.at(i*2+1);
            real2 = inputI.at(i*2); im2 = inputI.at(i*2+1);

            // additionally we have to devide by 2.0 because we get the amplitude information
            // rather then energy information
            resultReal = ((real1 * real2) + (im1 * im2)) / 2.0;
            resultIm = ((real1 * im2) - (real2 *im1)) / 2.0;

            resultListP.append(resultReal);
            resultListQ.append(resultIm);
            resultListS.append(sqrt((resultReal * resultReal) + (resultIm * resultIm)));
        }

        m_ppActValue->setValue(QVariant::fromValue<QList<double> >(resultListP));
        m_pqActValue->setValue(QVariant::fromValue<QList<double> >(resultListQ));
        m_psActValue->setValue(QVariant::fromValue<QList<double> >(resultListS));
    }
}

}
