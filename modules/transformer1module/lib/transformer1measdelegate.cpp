#include "transformer1measdelegate.h"
#include "vfmodulecomponent.h"
#include <useratan.h>

namespace  TRANSFORMER1MODULE
{

cTransformer1MeasDelegate::cTransformer1MeasDelegate(VfModuleComponent *acttrfError, VfModuleComponent *acttrfAngleError, VfModuleComponent *acttrfRatio, VfModuleComponent *actinsec, VfModuleComponent *actixsec, VfModuleComponent *ixprim, bool withSignal)
    :m_pActTransformerError(acttrfError), m_pActTransformerAngleError(acttrfAngleError), m_pActTransformerRatio(acttrfRatio), m_pActINSecondary(actinsec), m_pActIXSecondary(actixsec), m_pActIXPrimary(ixprim), m_bSignal(withSignal)
{
}


void cTransformer1MeasDelegate::actValueInput1(QVariant val)
{
    QList<double> list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        inp1 = std::complex<double>(list.at(0), list.at(1));

        // we forward primary input value
        m_pActINSecondary->setValue(fabs(inp1) * c1_sqrt2);

        // we compute vector as complex primary actual values
        m_fPrimVector = inp1 * (m_fPrimClampPrim / m_fPrimClampSec);

        computeOutput();
    }
}


void cTransformer1MeasDelegate::actValueInput2(QVariant val)
{
    QList<double> list;
    list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        inp2 = std::complex<double>(list.at(0), list.at(1));

        // we forward secondary input value
        m_pActIXSecondary->setValue(fabs(inp2) * c1_sqrt2);

        // we compute vector as complex primary actual values
        m_fSecVector = inp2 * (m_fSecClampPrim / m_fSecClampSec);
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

    double dError = ((target - dRatio) / target) * 100.0;
    m_pActTransformerError->setValue(dError);

    double dAngleError = userAtan(m_fPrimVector.imag(), m_fPrimVector.real()) - userAtan(m_fSecVector.imag(), m_fSecVector.real());
    m_pActTransformerAngleError->setValue(dAngleError);

    double IXPrim = fabs(inp2) * dRatio * c1_sqrt2;
    m_pActIXPrimary->setValue(IXPrim);

}


}
