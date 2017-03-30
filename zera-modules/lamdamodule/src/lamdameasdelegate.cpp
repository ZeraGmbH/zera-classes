#include <QDebug>
#include <QtGlobal>

#include <veinmoduleactvalue.h>

#include "debug.h"
#include "lamdameasdelegate.h"


namespace  LAMDAMODULE
{

cLamdaMeasDelegate::cLamdaMeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal)
    :m_pActValue(actvalue), m_bSignal(withSignal)
{
}


void cLamdaMeasDelegate::actValueInput1(QVariant val)
{
    input1 = val.toDouble();
    computeOutput();
}


void cLamdaMeasDelegate::actValueInput2(QVariant val)
{
    input2 = val.toDouble();
    if (m_bSignal)
        emit measuring(0);
    computeOutput();
    if (m_bSignal)
        emit measuring(1);
}


void cLamdaMeasDelegate::computeOutput()
{
    double lamda;

    if (input2 == 0)
    {
        lamda = qSNaN();
        m_pActValue->setValue(lamda);
#ifdef DEBUG
    QString ts;
    ts = QString("Lamda: %1").arg(lamda);
    qDebug() << ts;
#endif
        return;
    }

    lamda = input1 / input2;

    if (fabs(lamda) > 1.0000001)
        lamda = qSNaN();

#ifdef DEBUG
    QString ts;
    ts = QString("Lamda: %1").arg(lamda);
    qDebug() << ts;
#endif

    m_pActValue->setValue(lamda);
}


}
