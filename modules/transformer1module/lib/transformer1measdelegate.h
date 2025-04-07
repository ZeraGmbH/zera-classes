#ifndef TRANSFORMER1MEASDELEGATE
#define TRANSFORMER1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>
#include <complex>

class VfModuleComponent;

namespace TRANSFORMER1MODULE
{

const double c1_sqrt2 = 0.707106781;

class cTransformer1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cTransformer1MeasDelegate(){}
    cTransformer1MeasDelegate(VfModuleComponent *acttrfError, VfModuleComponent *acttrfAngleError, VfModuleComponent *acttrfRatio,
                              VfModuleComponent *actinsec, VfModuleComponent *actixsec, VfModuleComponent *ixprim, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);
    void setDutPrim(QVariant val);
    void setDutSec(QVariant val);
    void setPrimClampPrim(QVariant val);
    void setPrimClampSec(QVariant val);
    void setSecClampPrim(QVariant val);
    void setSecClampSec(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleComponent *m_pActTransformerError; // output is transmission error
    VfModuleComponent *m_pActTransformerAngleError; // and angle error
    VfModuleComponent *m_pActTransformerRatio; // and computed actual ratio
    VfModuleComponent *m_pActINSecondary;
    VfModuleComponent *m_pActIXSecondary;
    VfModuleComponent *m_pActIXPrimary;

    bool m_bSignal;

    std::complex<double> inp1, inp2;

    std::complex<double> m_fPrimVector;
    std::complex<double> m_fSecVector;
    double m_fDutPrim;
    double m_fDutSec;
    double m_fPrimClampPrim;
    double m_fPrimClampSec;
    double m_fSecClampPrim;
    double m_fSecClampSec;

    void computeOutput();
};

}


#endif // TRANSFORMER1

