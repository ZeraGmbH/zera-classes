#ifndef EFFICIENCY1MEASDELEGATE
#define EFFICIENCY1MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class cVeinModuleActvalue;

namespace EFFICIENCY1MODULE
{

class cEfficiency1MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cEfficiency1MeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

signals:
    void measuring(int);

private:
    cVeinModuleActvalue *m_pActValue;
    bool m_bSignal;
    QList<double> input1;
    QList<double> input2;

    void computeOutput();
};

}


#endif // EFFICIENCY1

