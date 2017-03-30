#ifndef LAMDAMEASDELEGATE
#define LAMDAMEASDELEGATE

#include <QObject>
#include <QVariant>
#include <QList>

class cVeinModuleActvalue;

namespace LAMDAMODULE
{

class cLamdaMeasDelegate : public QObject
{
    Q_OBJECT

public:
    cLamdaMeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal = false);

public slots:
    void actValueInput1(QVariant val);
    void actValueInput2(QVariant val);

signals:
    void measuring(int);

private:
    cVeinModuleActvalue *m_pActValue;
    bool m_bSignal;
    double input1;
    double input2;

    void computeOutput();
};

}


#endif // LAMDAMEASDELEGATE

