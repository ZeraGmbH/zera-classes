#ifndef LAMBDAMEASDELEGATE
#define LAMBDAMEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class VfModuleActvalue;

namespace LAMBDAMODULE
{

class cLambdaMeasDelegate : public QObject
{
    Q_OBJECT

public:
    cLambdaMeasDelegate(VfModuleActvalue *actvalue, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);
    void actValueActivePowerMeasMode(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleActvalue *m_pActValue;
    bool m_bSignal;
    double input1;
    double input2;
    bool m_MeasMode3LW = false;

    void computeOutput();
};

}


#endif // LAMBDAMEASDELEGATE

