#include "lambdacalcdelegate.h"

LambdaCalcDelegate::LambdaCalcDelegate(bool activePowerMeasModeAvail)
    : m_activePowerMeasModeAvail{activePowerMeasModeAvail}
{
}

PhaseSumValues LambdaCalcDelegate::getLambdaValues()
{
    return m_lambdaValues;
}

void LambdaCalcDelegate::onActivePower1Change(double power)
{
    m_activePowerValues.phases[0] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePower2Change(double power)
{
    m_activePowerValues.phases[1] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePower3Change(double power)
{
    m_activePowerValues.phases[2] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePowerSumChange(double power)
{
    m_activePowerValues.sum = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onApparentPower1Change(double power)
{
    m_apparentPowerValues.phases[0] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onApparentPower2Change(double power)
{
    m_apparentPowerValues.phases[1] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onApparentPower3Change(double power)
{
    m_apparentPowerValues.phases[2] = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onApparentPowerSumChange(double power)
{
    m_apparentPowerValues.sum = power;
    updateLambdaValues();
}

void LambdaCalcDelegate::onActivePowerMeasModeChange(QString measMode)
{
    if (m_activePowerMeasModeAvail) {
        m_activePowerMeasMode = measMode;
        updateLambdaValues();
    }
}

void LambdaCalcDelegate::onActivePowerPhaseMaskChange(QString phaseMask)
{
    if (m_activePowerMeasModeAvail) {
        m_activePowerPhaseMask = phaseMask;
        updateLambdaValues();
    }
}

void LambdaCalcDelegate::updateLambdaValues()
{
    m_lambdaValues = LambdaCalculator::calculateAllLambdas(m_activePowerValues, m_apparentPowerValues, m_activePowerMeasMode, m_activePowerPhaseMask);
}
