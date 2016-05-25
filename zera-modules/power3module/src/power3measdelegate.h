#ifndef POWER3MEASDELEGATE
#define POWER3MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class cVeinModuleActvalue;

namespace POWER3MODULE
{

class cPower3MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cPower3MeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal = false);

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


#endif // POWER3MEASDELEGATE

