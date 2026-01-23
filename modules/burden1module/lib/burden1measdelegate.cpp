#include "burden1measdelegate.h"
#include "vfmodulecomponent.h"
#include <useratan.h>
#include <complex>

namespace  BURDEN1MODULE
{

cBurden1MeasDelegate::cBurden1MeasDelegate(VeinStorage::AbstractComponentPtr inputVectorU,
                                           VeinStorage::AbstractComponentPtr inputVectorI,
                                           VfModuleComponent *actburden,
                                           VfModuleComponent *actpowerfactor,
                                           VfModuleComponent *actrelburden,
                                           const QString &mode) :
    m_inputVectorU(inputVectorU),
    m_inputVectorI(inputVectorI),
    m_pActBurden(actburden),
    m_pActPowerFactor(actpowerfactor),
    m_pActRelativeBurden(actrelburden),
    m_sMode(mode)
{
}

double cBurden1MeasDelegate::calcWireResistence(double wireLenMeter, double wireCrossSectionMillimeterSquare)
{
    constexpr double copperConductivity = 56;
    return wireLenMeter / (wireCrossSectionMillimeterSquare * copperConductivity);
}

void cBurden1MeasDelegate::setNominalBurden(const QVariant &val)
{
    m_fNominalBurden = val.toDouble();
}

void cBurden1MeasDelegate::setNominalRange(const QVariant &val)
{
    m_fNominalRange = val.toDouble();
}

void cBurden1MeasDelegate::setNominalRangeFactor(const QVariant &val)
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

void cBurden1MeasDelegate::setWireLength(const QVariant &val)
{
    m_fWireLength = val.toDouble();
}

void cBurden1MeasDelegate::setWireCrosssection(const QVariant &val)
{
    m_fWireCrosssection = val.toDouble();
}

void cBurden1MeasDelegate::computeOutput()
{
    QList<double> listU = m_inputVectorU->getValue().value<QList<double>>();
    std::complex<double> vectorU = std::complex<double>(listU.at(0), listU.at(1));;
    QList<double> listI = m_inputVectorI->getValue().value<QList<double>>();
    std::complex<double> vectorI = std::complex<double>(listI.at(0), listI.at(1));

    double Rwire = calcWireResistence(m_fWireLength, m_fWireCrosssection);
    double ueff = fabs(vectorU) / 1.41421356;
    double ieff = fabs(vectorI) / 1.41421356;

    // computation of burden, powerfactor and rel. burden !!! vectors are complex !!!
    double nominalRange=m_fNominalRange*m_fNominalRangeFactor;

    if (m_sMode == "V")
        m_fActBurden = ((nominalRange * nominalRange) * ieff) / (ueff + (Rwire * ieff));
    else
        m_fActBurden = (nominalRange * nominalRange) * ((ueff/ieff) + Rwire);

    double deltaW = userAtan(vectorI.imag(), vectorI.real()) - userAtan(vectorU.imag(), vectorU.real());
    deltaW = deltaW * M_PI / 180.0;

    m_fActPowerFactor = cos(deltaW);

    m_fActRelativeBurden = m_fActBurden *100.0 / m_fNominalBurden;

    m_pActBurden->setValue(m_fActBurden);
    m_pActPowerFactor->setValue(m_fActPowerFactor);
    m_pActRelativeBurden->setValue(m_fActRelativeBurden);
}

}
