#include <QDebug>
#include <QtGlobal>

#include <veinmoduleactvalue.h>

#include "debug.h"
#include "lambdameasdelegate.h"


namespace  LAMBDAMODULE
{

cLambdaMeasDelegate::cLambdaMeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal)
    :m_pActValue(actvalue), m_bSignal(withSignal)
{
}


void cLambdaMeasDelegate::actValueInput1(QVariant val)
{
    input1 = val.toDouble();
    computeOutput();
}


void cLambdaMeasDelegate::actValueInput2(QVariant val)
{
    input2 = val.toDouble();
    if (m_bSignal)
        emit measuring(0);
    computeOutput();
    if (m_bSignal)
        emit measuring(1);
}


void cLambdaMeasDelegate::computeOutput()
{
    double lambda;

    if (input2 == 0)
    {
        lambda = qSNaN();
        m_pActValue->setValue(lambda);
#ifdef DEBUG
    QString ts;
    ts = QString("Lambda: %1").arg(lambda);
    qDebug() << ts;
#endif
        return;
    }

    lambda = input1 / input2;

    if (fabs(lambda) > 1.0000001)
        lambda = qSNaN();

#ifdef DEBUG
    QString ts;
    ts = QString("Lambda: %1").arg(lambda);
    qDebug() << ts;
#endif

    m_pActValue->setValue(lambda);
}


}
