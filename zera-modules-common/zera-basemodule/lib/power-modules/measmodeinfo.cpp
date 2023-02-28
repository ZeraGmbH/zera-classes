#include "measmodeinfo.h"

cMeasModeInfo::cMeasModeInfo(QString name, QString actvalname, QString unitname, int powmode, int code) :
    m_sName(name),
    m_sActvalName(actvalname),
    m_sUnitName(unitname),
    m_nPowerMode(powmode),
    m_nCode(code)
{
}

bool cMeasModeInfo::isValid()
{
    return !m_sName.isEmpty();
}

QString cMeasModeInfo::getName() const
{
    return m_sName;
}

QString cMeasModeInfo::getActvalName() const
{
    return m_sActvalName;
}

QString cMeasModeInfo::getUnitName() const
{
    return m_sUnitName;
}

quint8 cMeasModeInfo::getPowerMode() const
{
    return m_nPowerMode;

}

quint8 cMeasModeInfo::getCode () const
{
    return m_nCode;
}

