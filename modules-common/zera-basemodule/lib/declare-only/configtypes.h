#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <QString>

struct boolParameter
{
    QString m_sKey;
    quint8 m_nActive = 0; // active or not 1,0
};

struct intParameter
{
    QString m_sKey;
    quint32 m_nValue = 0;
};

struct doubleParameter
{
    QString m_sKey;
    double m_fValue = 0.0;
};

struct stringParameter
{
    QString m_sKey;
    QString m_sValue;
};

#endif // CONFIGTYPES_H