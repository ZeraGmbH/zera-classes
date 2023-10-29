#include <QDebug>
#include <math.h>

#include <vfmoduleactvalue.h>
#include <useratan.h>

#include "burden1measdelegate.h"


namespace  BURDEN1MODULE
{

cBurden1MeasDelegate::cBurden1MeasDelegate(VfModuleActvalue *actburden, VfModuleActvalue *actpowerfactor, VfModuleActvalue *actrelburden, QString mode, bool withSignal)
    :m_pActBurden(actburden), m_pActPowerFactor(actpowerfactor), m_pActRelativeBurden(actrelburden), m_sMode(mode), m_bSignal(withSignal)
{
}


void cBurden1MeasDelegate::actValueInput1(QVariant val)
{
    QList<double> list;
    list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        // we compute vector as complex primary actual values
        m_fVoltageVector = std::complex<double>(list.at(0), list.at(1));
        computeOutput();
    }
}


void cBurden1MeasDelegate::actValueInput2(QVariant val)
{
    QList<double> list;
    list = val.value<QList<double> >();
    if (list.count() >= 2) // normaly this is true, but we test to avoid crashing
    {
        // we compute vector as complex primary actual values
        m_fCurrentVector = std::complex<double>(list.at(0), list.at(1));
        if (m_bSignal)
            emit measuring(0);

        computeOutput();

        if (m_bSignal)
            emit measuring(1);
    }
}


void cBurden1MeasDelegate::setNominalBurden(QVariant val)
{
    m_fNominalBurden = val.toDouble();
}


void cBurden1MeasDelegate::setNominalRange(QVariant val)
{
    m_fNominalRange = val.toDouble();
}


void cBurden1MeasDelegate::setNominalRangeFactor(QVariant val)
{
    m_sNominalRangeFactor = val.toString();
    if (m_sNominalRangeFactor == "1")
        m_fNominalRangeFactor = 1.0;
    if (m_sNominalRangeFactor == "sqrt(3)")
        m_fNominalRangeFactor = sqrt(3.0);
    if (m_sNominalRangeFactor == "1/sqrt(3)")
        m_fNominalRangeFactor = 1.0 / sqrt(3.0);
    if (m_sNominalRangeFactor == "1/3")
        m_fNominalRangeFactor = 1.0/3.0;
}


void cBurden1MeasDelegate::setWireLength(QVariant val)
{
    m_fWireLength = val.toDouble();
}


void cBurden1MeasDelegate::setWireCrosssection(QVariant val)
{
    m_fWireCrosssection = val.toDouble();
}


void cBurden1MeasDelegate::computeOutput()
{
    double Rwire, ueff, ieff;

    Rwire = (1.0/56) * m_fWireLength / m_fWireCrosssection;
    ueff = fabs(m_fVoltageVector) / 1.41421356;
    ieff = fabs(m_fCurrentVector) / 1.41421356;

    // computation of burden, powerfactor and rel. burden !!! vectors are complex !!!

    double nominalRange=m_fNominalRange*m_fNominalRangeFactor;

    if (m_sMode == "V")
    {
        m_fActBurden = ((nominalRange * nominalRange) * ieff) / (ueff + (Rwire * ieff));
    }
    else
    {
        m_fActBurden = (nominalRange * nominalRange) * ((ueff/ieff) + Rwire);
    }

    double deltaW = userAtan(m_fCurrentVector.imag(), m_fCurrentVector.real()) - userAtan(m_fVoltageVector.imag(), m_fVoltageVector.real());
    deltaW = deltaW * 3.1415926 / 180.0;

    m_fActPowerFactor = cos(deltaW);

    m_fActRelativeBurden = m_fActBurden *100.0 / m_fNominalBurden;

    m_pActBurden->setValue(m_fActBurden);
    m_pActPowerFactor->setValue(m_fActPowerFactor);
    m_pActRelativeBurden->setValue(m_fActRelativeBurden);

}


}
