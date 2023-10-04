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
    cLambdaMeasDelegate(VfModuleActvalue *actvalue, int phaseNumber);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);
    void actValueActivePowerMeasMode(QVariant val);
    void actValueActivePowerMeasPhases(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleActvalue *m_pActValue;
    bool m_bSignal = false;
    double input1 = 0.0;
    double input2 = 0.0;
    bool m_phaseActive = true;
    bool m_MeasMode3LW = false;
    int m_phaseNumber = 0;

    void computeOutput();
};

}


#endif // LAMBDAMEASDELEGATE

