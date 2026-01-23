#ifndef POWER3MEASDELEGATE
#define POWER3MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class VfModuleComponent;

namespace POWER3MODULE
{

class cPower3MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cPower3MeasDelegate(VfModuleComponent *pactvalue, VfModuleComponent *qactvalue, VfModuleComponent *sactvalue, bool withSignal = false);

public slots:
    void actValueInputU(QVariant val);
    void actValueInputI(QVariant val);

signals:
    void measuring(int);

private:
    VfModuleComponent *m_ppActValue;
    VfModuleComponent *m_pqActValue;
    VfModuleComponent *m_psActValue;
    bool m_bSignal;
    QList<double> input1;
    QList<double> input2;

    void computeOutput();
};

}


#endif // POWER3MEASDELEGATE

