#ifndef POWER3MEASDELEGATE
#define POWER3MEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class VeinEntity;
class VeinPeer;

namespace POWER3MODULE
{

class cPower3MeasDelegate : public QObject
{
    Q_OBJECT

public:
    cPower3MeasDelegate(VeinPeer *peer, VeinEntity *output);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

private:
    VeinPeer *m_pPeer;
    VeinEntity *m_pOutput;
    QList<double> input1;
    QList<double> input2;

    void computeOutput();
};

}


#endif // POWER3MEASDELEGATE

