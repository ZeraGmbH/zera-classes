#include <QDebug>
#include <QtGlobal>

#include <veinmoduleactvalue.h>
#include <math.h>

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

    if (input1 == 0)
        lambda = 0;
    else
    {
        if (input2 == 0)
        {
            lambda = qSNaN();
        }
        else
        {
            lambda = input1 / input2;
            if (fabs(lambda) > 1.0)
                lambda = 1.0;
            // we would perhaps better create a complete new measurement module
            // that computes P and S and then lambda
        }
    }

    m_pActValue->setValue(lambda);
}


}
