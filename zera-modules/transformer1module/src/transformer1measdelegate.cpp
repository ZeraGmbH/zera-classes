#include <QDebug>

#include <veinmoduleactvalue.h>
#include <useratan.h>

#include "debug.h"
#include "transformer1measdelegate.h"


namespace  TRANSFORMER1MODULE
{

cTransformer1MeasDelegate::cTransformer1MeasDelegate(cVeinModuleActvalue *acttrfError, cVeinModuleActvalue *acttrfAngleError, cVeinModuleActvalue *acttrfRatio, bool withSignal)
    :m_pActTransformerError(acttrfError), m_pActTransformerAngleError(acttrfAngleError), m_pActTransformerRatio(acttrfRatio), m_bSignal(withSignal)
{
}


void cTransformer1MeasDelegate::actValueInput1(QVariant val)
{
    QList<double> list;
    list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        // we compute vector as complex primary actual values
        m_fPrimVector = complex(list.at(0), list.at(1)) * (m_fPrimClampPrim / m_fPrimClampSec);
        computeOutput();
    }
}


void cTransformer1MeasDelegate::actValueInput2(QVariant val)
{
    QList<double> list;
    list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        // we compute vector as complex primary actual values
        m_fSecVector = complex(list.at(0), list.at(1)) * (m_fSecClampPrim / m_fSecClampSec);
        if (m_bSignal)
            emit measuring(0);

        computeOutput();

        if (m_bSignal)
            emit measuring(1);
    }
}


void cTransformer1MeasDelegate::setDutPrim(QVariant val)
{
    m_fDutPrim = val.toDouble();
}


void cTransformer1MeasDelegate::setDutSec(QVariant val)
{
    m_fDutSec = val.toDouble();
}


void cTransformer1MeasDelegate::setPrimClampPrim(QVariant val)
{
    m_fPrimClampPrim = val.toDouble();
}


void cTransformer1MeasDelegate::setPrimClampSec(QVariant val)
{
    m_fPrimClampSec = val.toDouble();
}


void cTransformer1MeasDelegate::setSecClampPrim(QVariant val)
{
    m_fSecClampPrim = val.toDouble();
}


void cTransformer1MeasDelegate::setSecClampSec(QVariant val)
{
    m_fSecClampSec = val.toDouble();
}


void cTransformer1MeasDelegate::computeOutput()
{
    double target = m_fDutPrim / m_fDutSec;

    // computation of ration, transmission error and phase angle error !!! complex !!!

    double dRatio = fabs(m_fPrimVector) / fabs (m_fSecVector);
    m_pActTransformerRatio->setValue(dRatio);

    double dError = (dRatio - target) / target;
    m_pActTransformerError->setValue(dError);

    double dAngleError = userAtan(m_fPrimVector.im(), m_fPrimVector.re()) - userAtan(m_fSecVector.im(), m_fSecVector.re());
    m_pActTransformerAngleError->setValue(dAngleError);

#ifdef DEBUG
    QString ts;
    ts = QString("TransmissionError: %1; Angle: %2; Ratio: %3").arg(dError).arg(dAngleError).arg(dRatio);
    qDebug() << ts;
#endif

}


}
