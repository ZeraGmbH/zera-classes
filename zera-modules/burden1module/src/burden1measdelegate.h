#ifndef BURDEN1MEASDELEGATE
#define BURDEN1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>
#include <complex.h>

class VfModuleActvalue;

namespace BURDEN1MODULE
{

class cBurden1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cBurden1MeasDelegate(VfModuleActvalue *actburden, VfModuleActvalue *actpowerfactor, VfModuleActvalue *actrelburden, QString mode, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);
    void setNominalBurden(QVariant val);
    void setNominalRange(QVariant val);
    void setNominalRangeFactor(QVariant val);
    void setWireLength(QVariant val);
    void setWireCrosssection(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleActvalue *m_pActBurden; // output is actual burden corrected to nominal burden
    VfModuleActvalue *m_pActPowerFactor; // and angle cosß
    VfModuleActvalue *m_pActRelativeBurden; // % value of nominal burden
    QString m_sMode;

    bool m_bSignal;

    complex m_fVoltageVector;
    complex m_fCurrentVector;
    double m_fNominalBurden;
    double m_fNominalRange;
    QString m_sNominalRangeFactor;
    double m_fNominalRangeFactor;
    double m_fWireLength;
    double m_fWireCrosssection;

    double m_fActBurden;
    double m_fActPowerFactor;
    double m_fActRelativeBurden;

    void computeOutput();
};

}


#endif // BURDEN1MEASDELEGATE

