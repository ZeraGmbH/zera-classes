#ifndef TRANSFORMER1MEASDELEGATE
#define TRANSFORMER1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>
#include <complex.h>

class VfModuleActvalue;

namespace TRANSFORMER1MODULE
{

const double c1_sqrt2 = 0.707106781;

class cTransformer1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cTransformer1MeasDelegate(){}
    cTransformer1MeasDelegate(VfModuleActvalue *acttrfError, VfModuleActvalue *acttrfAngleError, VfModuleActvalue *acttrfRatio,
                              VfModuleActvalue *actinsec, VfModuleActvalue *actixsec, VfModuleActvalue *ixprim, bool withSignal = false);

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
    VfModuleActvalue *m_pActTransformerError; // output is transmission error
    VfModuleActvalue *m_pActTransformerAngleError; // and angle error
    VfModuleActvalue *m_pActTransformerRatio; // and computed actual ratio
    VfModuleActvalue *m_pActINSecondary;
    VfModuleActvalue *m_pActIXSecondary;
    VfModuleActvalue *m_pActIXPrimary;

    bool m_bSignal;

    complex inp1, inp2;

    complex m_fPrimVector;
    complex m_fSecVector;
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

