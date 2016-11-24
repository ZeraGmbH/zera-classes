#ifndef TRANSFORMER1MEASDELEGATE
#define TRANSFORMER1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>
#include <complex.h>

class cVeinModuleActvalue;

namespace TRANSFORMER1MODULE
{

class cTransformer1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cTransformer1MeasDelegate(){}
    cTransformer1MeasDelegate(cVeinModuleActvalue *acttrfError, cVeinModuleActvalue *acttrfAngleError, cVeinModuleActvalue *acttrfRatio, bool withSignal = false);

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
    cVeinModuleActvalue *m_pActTransformerError; // output is transmission error
    cVeinModuleActvalue *m_pActTransformerAngleError; // and angle error
    cVeinModuleActvalue *m_pActTransformerRatio; // and computed actual ratio

    bool m_bSignal;

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

