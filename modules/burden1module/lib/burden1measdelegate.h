#ifndef BURDEN1MEASDELEGATE
#define BURDEN1MEASDELEGATE

#include <vs_abstractcomponent.h>

class VfModuleComponent;

namespace BURDEN1MODULE
{

class cBurden1MeasDelegate : public QObject
{
    Q_OBJECT
public:
    cBurden1MeasDelegate(VeinStorage::AbstractComponentPtr inputVectorU,
                         VeinStorage::AbstractComponentPtr inputVectorI,
                         VfModuleComponent *actburden,
                         VfModuleComponent *actpowerfactor,
                         VfModuleComponent *actrelburden,
                         const QString &mode);
    static double calcWireResistence(double wireLenMeter, double wireCrossSectionMillimeterSquare);
    void computeOutput();

public slots:
    void setNominalBurden(const QVariant &val);
    void setNominalRange(const QVariant &val);
    void setNominalRangeFactor(const QVariant &val);
    void setWireLength(const QVariant &val);
    void setWireCrosssection(const QVariant &val);
private:
    VeinStorage::AbstractComponentPtr m_inputVectorU;
    VeinStorage::AbstractComponentPtr m_inputVectorI;

    VfModuleComponent *m_pActBurden; // output is actual burden corrected to nominal burden
    VfModuleComponent *m_pActPowerFactor; // and angle cosß
    VfModuleComponent *m_pActRelativeBurden; // % value of nominal burden
    QString m_sMode;

    double m_fNominalBurden;
    double m_fNominalRange;
    QString m_sNominalRangeFactor;
    double m_fNominalRangeFactor;
    double m_fWireLength;
    double m_fWireCrosssection;

    double m_fActBurden;
    double m_fActPowerFactor;
    double m_fActRelativeBurden;
};

}


#endif // BURDEN1MEASDELEGATE

