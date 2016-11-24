#ifndef EFFICIENCY1MEASDELEGATE
#define EFFICIENCY1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QHash>

class cVeinModuleActvalue;
class cVeinModuleComponentInput;

namespace EFFICIENCY1MODULE
{

class cEfficiency1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cEfficiency1MeasDelegate(){}
    cEfficiency1MeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal = false);

    void addInputPowerValue(cVeinModuleComponentInput *input);
    void addOutputPowerValue(cVeinModuleComponentInput *input);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

signals:
    void measuring(int);

private:
    cVeinModuleActvalue *m_pActValue;
    bool m_bSignal;
    QHash<cVeinModuleComponentInput*, double> input1Hash;
    QHash<cVeinModuleComponentInput*, double> input2Hash;
    cVeinModuleComponentInput *lastInput;
    double m_fEfficiency;

    void computeOutput();
};

}


#endif // EFFICIENCY1

