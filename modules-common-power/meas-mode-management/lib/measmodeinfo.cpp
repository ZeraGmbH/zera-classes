#include "measmodeinfo.h"
#include "measmodecatalog.h"

cMeasModeInfo::cMeasModeInfo(QString name, QString actvalname, QString unitname, measmodes code) :
    m_sName(name),
    m_sActvalName(actvalname),
    m_sUnitName(unitname),
    m_nCode(code)
{
}

bool cMeasModeInfo::isValid() const
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

measmodes cMeasModeInfo::getCode() const
{
    return m_nCode;
}

bool cMeasModeInfo::isThreeWire() const
{
    return MeasModeCatalog::getThreeWireModes().contains(m_nCode);
}
