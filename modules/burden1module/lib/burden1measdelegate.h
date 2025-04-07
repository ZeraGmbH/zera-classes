#ifndef BURDEN1MEASDELEGATE
#define BURDEN1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>
#include <complex>

class VfModuleComponent;

namespace BURDEN1MODULE
{

class cBurden1MeasDelegate : public QObject
{
    Q_OBJECT
public:
    cBurden1MeasDelegate(VfModuleComponent *actburden, VfModuleComponent *actpowerfactor, VfModuleComponent *actrelburden, QString mode, bool withSignal = false);
    static double calcWireResistence(double wireLenMeter, double wireCrossSectionMillimeterSquare);
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
    void computeOutput();
    VfModuleComponent *m_pActBurden; // output is actual burden corrected to nominal burden
    VfModuleComponent *m_pActPowerFactor; // and angle cosß
    VfModuleComponent *m_pActRelativeBurden; // % value of nominal burden
    QString m_sMode;

    bool m_bSignal;

    std::complex<double> m_fVoltageVector;
    std::complex<double> m_fCurrentVector;
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

