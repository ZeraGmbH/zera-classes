#ifndef EFFICIENCY1MEASDELEGATE
#define EFFICIENCY1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QHash>

class VfModuleActvalue;
class VfModuleComponentInput;

namespace EFFICIENCY1MODULE
{

class cEfficiency1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cEfficiency1MeasDelegate(){}
    cEfficiency1MeasDelegate(VfModuleActvalue *actvalue, bool withSignal = false);

    void addInputPowerValue(VfModuleComponentInput *input);
    void addOutputPowerValue(VfModuleComponentInput *input);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleActvalue *m_pActValue;
    bool m_bSignal;
    QHash<VfModuleComponentInput*, double> input1Hash;
    QHash<VfModuleComponentInput*, double> input2Hash;
    VfModuleComponentInput *lastInput;
    double m_fEfficiency;

    void computeOutput();
};

}


#endif // EFFICIENCY1

