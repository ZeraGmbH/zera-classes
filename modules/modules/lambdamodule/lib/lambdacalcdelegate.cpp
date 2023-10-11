#include "lambdacalcdelegate.h"

LambdaCalcDelegate::LambdaCalcDelegate(bool activePowerMeasModeAvail)
    : m_activePowerMeasModeAvail{activePowerMeasModeAvail}
{
}

PhaseSumValues LambdaCalcDelegate::getLambdaValues()
{
    return m_lambdaValues;
}

void LambdaCalcDelegate::onActivePower1Change(QVariant power)
{
    m_activePowerValues.phases[0] = power.toDouble();
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePower2Change(QVariant power)
{
    m_activePowerValues.phases[1] = power.toDouble();
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePower3Change(QVariant power)
{
    m_activePowerValues.phases[2] = power.toDouble();
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePowerSumChange(QVariant power)
{
    m_activePowerValues.sum = power.toDouble();
    updateLambdaValues();
}

void LambdaCalcDelegate::onApparentPower1Change(QVariant power)
{
    m_apparentPowerValues.phases[0] = power.toDouble();
    emit measuring(0);
    updateLambdaValues();
    emit measuring(1);
}

void LambdaCalcDelegate::onApparentPower2Change(QVariant power)
{
    m_apparentPowerValues.phases[1] = power.toDouble();
    emit measuring(0);
    updateLambdaValues();
    emit measuring(1);
}

void LambdaCalcDelegate::onApparentPower3Change(QVariant power)
{
    m_apparentPowerValues.phases[2] = power.toDouble();
    emit measuring(0);
    updateLambdaValues();
    emit measuring(1);
}

void LambdaCalcDelegate::onApparentPowerSumChange(QVariant power)
{
    m_apparentPowerValues.sum = power.toDouble();
    updateLambdaValues();
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
    m_lambdaValues = LambdaCalculator::calculateAllLambdas(m_activePowerValues, m_apparentPowerValues, m_activePowerMeasMode, m_activePowerPhaseMask);
}
