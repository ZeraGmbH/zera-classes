#include "measmodeinfo.h"


cMeasModeInfo::cMeasModeInfo(QString name, QString actvalname, QString unitname, int powmode, int code)
    :m_sName(name), m_sActvalName(actvalname), m_sUnitName(unitname), m_nPowerMode(powmode), m_nCode(code)
{
}


QString cMeasModeInfo::getName()
{
    return m_sName;
}


QString cMeasModeInfo::getActvalName()
{
    return m_sActvalName;
}


QString cMeasModeInfo::getUnitName()
{
    return m_sUnitName;
}


quint8 cMeasModeInfo::getPowerMode()
{
    return m_nPowerMode;

}


quint8 cMeasModeInfo::getCode()
{
    return m_nCode;
}

