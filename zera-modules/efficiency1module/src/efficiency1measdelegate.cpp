#include <QDebug>

#include <veinmoduleactvalue.h>
#include <veinmodulecomponentinput.h>

#include "debug.h"
#include "efficiency1measdelegate.h"


namespace  EFFICIENCY1MODULE
{

cEfficiency1MeasDelegate::cEfficiency1MeasDelegate(cVeinModuleActvalue *actvalue, bool withSignal)
    :m_pActValue(actvalue), m_bSignal(withSignal)
{
}


void cEfficiency1MeasDelegate::addInputPowerValue(cVeinModuleComponentInput *input)
{
    input1Hash[input] = 0.0;
    lastInput = input;
    connect(input, &cVeinModuleComponentInput::sigValueChanged, this, &cEfficiency1MeasDelegate::actValueInput1);
}


void cEfficiency1MeasDelegate::addOutputPowerValue(cVeinModuleComponentInput *input)
{
    input2Hash[input] = 0.0;
    lastInput = input;
    connect(input, &cVeinModuleComponentInput::sigValueChanged, this, &cEfficiency1MeasDelegate::actValueInput2);
}


void cEfficiency1MeasDelegate::actValueInput1(QVariant val)
{
    cVeinModuleComponentInput* input = qobject_cast<cVeinModuleComponentInput*>(QObject::sender());
    input1Hash.remove(input);
    input1Hash[input] = val.toDouble();
    if (input == lastInput)
        computeOutput();
}


void cEfficiency1MeasDelegate::actValueInput2(QVariant val)
{
    cVeinModuleComponentInput* input = qobject_cast<cVeinModuleComponentInput*>(QObject::sender());
    input2Hash.remove(input);
    input2Hash[input] = val.toDouble();
    if (input == lastInput)
        computeOutput();
}


void cEfficiency1MeasDelegate::computeOutput()
{
    if (m_bSignal)
        emit measuring(0);

    double inputPower;
    double outputPower;

    QList<double> inputlist;

    inputlist = input1Hash.values();
    inputPower = 0.0;
    for (int i = 0; i < inputlist.count(); i++)
        inputPower += inputlist.at(i);

    inputlist = input2Hash.values();
    outputPower = 0.0;
    for (int i = 0; i < inputlist.count(); i++)
        outputPower += inputlist.at(i);

    m_fEfficiency = outputPower / inputPower;

    m_pActValue->setValue(QVariant(m_fEfficiency));

    if (m_bSignal)
        emit measuring(1);

}


}
