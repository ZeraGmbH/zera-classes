#include "measmodeinfo.h"
#include "measmodecatalog.h"

cMeasModeInfo::cMeasModeInfo(const QString &name,
                             const QString &actvalname,
                             const QString &unitname,
                             measmodes code) :
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

sumtypes cMeasModeInfo::getSumTypeCode() const
{
    switch (m_nCode)
    {
    case m4lsg:
    case m3lsg:
    case m2lsg:
    case mXlsg:
        return sum_phase_pq_geom;
    default:
        return sum_phasevals;
    }
}

bool cMeasModeInfo::isThreeWire() const
{
    return MeasModeCatalog::getThreeWireModes().contains(m_nCode);
}
