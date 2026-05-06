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
    cBurden1MeasDelegate(const VeinStorage::AbstractComponentPtr &inputVectorU,
                         const VeinStorage::AbstractComponentPtr &inputVectorI,
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

    VfModuleComponent *m_pActBurden = nullptr;          // output is actual burden corrected to nominal burden
    VfModuleComponent *m_pActPowerFactor = nullptr;     // and angle cosß
    VfModuleComponent *m_pActRelativeBurden = nullptr;  // % value of nominal burden
    QString m_sMode;

    double m_fNominalBurden = 0.0;
    double m_fNominalRange = 0.0;
    QString m_sNominalRangeFactor;
    double m_fNominalRangeFactor = 0.0;
    double m_fWireLength = 0.0;
    double m_fWireCrosssection = 0.0;

    double m_fActBurden = 0.0;
    double m_fActPowerFactor = 0.0;
    double m_fActRelativeBurden = 0.0;
};

}


#endif // BURDEN1MEASDELEGATE

