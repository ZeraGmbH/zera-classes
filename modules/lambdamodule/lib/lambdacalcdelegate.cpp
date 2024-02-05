#include "lambdacalcdelegate.h"

LambdaCalcDelegate::LambdaCalcDelegate(bool activePowerMeasModeAvail,
                                       QList<VfModuleActvalue *> veinLambdaActValues,
                                       QList<VfModuleActvalue *> veinLoadTypeValues) :
    m_activePowerMeasModeAvail(activePowerMeasModeAvail),
    m_veinLambdaActValues(veinLambdaActValues),
    m_veinLoadTypeValues(veinLoadTypeValues)
{
}

PhaseSumValues LambdaCalcDelegate::getLambdaValues()
{
    return m_lambdaValues;
}

void LambdaCalcDelegate::handleActivePowerChange(int phaseOrSum, QVariant power)
{
    if(phaseOrSum >= 0 && phaseOrSum < m_activePowerValues.size()) {
        m_activePowerValues[phaseOrSum] = power.toDouble();
        updateLambdaValues();
    }
    else
        qWarning("handleActivePowerChange: Phase out of limits: %i", phaseOrSum);
}

void LambdaCalcDelegate::handleReactivePowerChange(int phaseOrSum, QVariant power)
{
    /*if(power.toFloat() >= 0)
        m_veinLoadTypeValues[phaseOrSum]->setValue("Ind");
    else
        m_veinLoadTypeValues[phaseOrSum]->setValue("Cap");*/

    if(phaseOrSum >= 0 && phaseOrSum < m_reactivePowerValues.size()) {
        m_reactivePowerValues[phaseOrSum] = power.toDouble();
        updateLambdaValues();
    }
    else
        qWarning("handleActivePowerChange: Phase out of limits: %i", phaseOrSum);
}

void LambdaCalcDelegate::handleApparentPowerChange(int phaseOrSum, QVariant power)
{
    if(phaseOrSum >= 0 && phaseOrSum < m_apparentPowerValues.size()) {
        m_apparentPowerValues[phaseOrSum] = power.toDouble();
        emit measuring(0);
        updateLambdaValues();
        emit measuring(1);
    }
    else
        qWarning("handleApparentPowerChange: Phase out of limits: %i", phaseOrSum);
}

void LambdaCalcDelegate::onActivePowerMeasModeChange(QVariant measMode)
{
    if (m_activePowerMeasModeAvail) {
        m_activePowerMeasMode = measMode.toString();
        updateLambdaValues();
    }
}

void LambdaCalcDelegate::onActivePowerPhaseMaskChange(QVariant phaseMask)
{
    if (m_activePowerMeasModeAvail) {
        m_activePowerPhaseMask = phaseMask.toString();
        updateLambdaValues();
    }
}

void LambdaCalcDelegate::updateLambdaValues()
{
    m_lambdaValues = LambdaCalculator::calculateAllLambdas(m_activePowerValues,
                                                           m_reactivePowerValues,
                                                           m_apparentPowerValues,
                                                           m_activePowerMeasMode,
                                                           m_activePowerPhaseMask);
    if (!m_veinLambdaActValues.isEmpty()) {
        m_veinLambdaActValues.last()->setValue(m_lambdaValues.sum);
        for(int i = 0; i < m_veinLambdaActValues.count() - 1; i++)
            m_veinLambdaActValues.at(i)->setValue(m_lambdaValues.phases.at(i));
    }
}
