#ifndef POWER3MEASDELEGATE
#define POWER3MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class VfModuleActvalue;

namespace POWER3MODULE
{

class cPower3MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cPower3MeasDelegate(VfModuleActvalue *pactvalue, VfModuleActvalue *qactvalue, VfModuleActvalue *sactvalue, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleActvalue *m_ppActValue;
    VfModuleActvalue *m_pqActValue;
    VfModuleActvalue *m_psActValue;
    bool m_bSignal;
    QList<double> input1;
    QList<double> input2;

    void computeOutput();
};

}


#endif // POWER3MEASDELEGATE

