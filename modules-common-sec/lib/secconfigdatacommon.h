#ifndef SECCONFIGDATACOMMON_H
#define SECCONFIGDATACOMMON_H

#include "configtypes.h"
#include <QString>
#include <QList>

struct TInputNameAlias
{
    QString inputName;
    QString alias;
};

struct TSecCommonInputConfigs
{
    quint8 m_inputCount = 0;
    QList<TInputNameAlias> m_inputList;
    stringParameter m_input;
};

struct TSecCommonLimitConfigs
{
    doubleParameter m_fUpperLimit;
    doubleParameter m_fLowerLimit;
};

#endif // SECCONFIGDATACOMMON_H
