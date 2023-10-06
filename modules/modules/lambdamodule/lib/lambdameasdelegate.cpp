#include "lambdameasdelegate.h"
#include <vfmoduleactvalue.h>
#include <math.h>
#include <QDebug>
#include <QtGlobal>

namespace  LAMBDAMODULE
{

cLambdaMeasDelegate::cLambdaMeasDelegate(VfModuleActvalue *actvalue, bool withSignal, int phaseNumber)
    :m_pActValue(actvalue), m_bSignal(withSignal), m_phaseNumber(phaseNumber)
{
}

bool cLambdaMeasDelegate::isPhaseActive(QVariant activePhaseMask, int phaseNumber)
{
    MModePhaseMask currentPhaseSelection(activePhaseMask.toString().toStdString());
    return currentPhaseSelection[MeasPhaseCount - 1 - phaseNumber];
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

void cLambdaMeasDelegate::actValueActivePowerMeasMode(QVariant val)
{
    m_MeasMode3LW = (val.toString() == "3LW");
    computeOutput();
}

void cLambdaMeasDelegate::actValueActivePowerMeasPhases(QVariant val)
{
    m_phaseActive = isPhaseActive(val, m_phaseNumber);
    computeOutput();
}

void cLambdaMeasDelegate::computeOutput()
{
    double lambda;

    if (m_MeasMode3LW || !m_phaseActive)
        lambda = qSNaN();
    else if (input1 == 0)
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
