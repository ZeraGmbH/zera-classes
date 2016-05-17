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
    cPower3MeasDelegate(cVeinModuleActvalue *actvalue);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

private:
    cVeinModuleActvalue *m_pActValue;
    QList<double> input1;
    QList<double> input2;

    void computeOutput();
};

}


#endif // POWER3MEASDELEGATE

