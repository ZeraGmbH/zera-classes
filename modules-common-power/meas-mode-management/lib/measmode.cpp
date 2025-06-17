#include "measmode.h"
#include "measmodecatalog.h"
#include "measmodemaskstringconverter.h"

MeasMode::MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter) :
    m_measModeInfo(MeasModeCatalog::getInfo(modeName)),
    m_dspSelectCode(dspSelectCode),
    m_measModePhaseSetter(std::move(measModePhaseSetter)),
    m_measSysCount(measSysCount)
{
#ifndef QT_NO_DEBUG
    qInfo("Add %s / DSP selection code %i", qPrintable(modeName), dspSelectCode);
#endif
}

QString MeasMode::getName() const
{
    return m_measModeInfo.getName();
}

int MeasMode::getDspSelectCode() const
{
    return m_dspSelectCode;
}

int MeasMode::getDspSumSelectCode() const
{
    return m_measModeInfo.getSumTypeCode();
}

bool MeasMode::hasVarMask() const
{
    if(isValid())
        return m_measModePhaseSetter->isVarMask();
    return false;
}

bool MeasMode::calcBinMask(QString mask, MModePhaseMask &binMask) const
{
    MModePhaseMask tmpMask;
    bool ok = isValid() && MeasModeMaskStringConverter::calcBinMask(mask, m_measSysCount, tmpMask);
    if(ok)
        binMask = tmpMask;
    return ok;
}

bool MeasMode::canChangeMask(QString mask) const
{
    MModePhaseMask binMask;
    bool ok = calcBinMask(mask, binMask);
    return ok && m_measModePhaseSetter->canChangeMask(binMask);
}

bool MeasMode::tryChangeMask(QString mask)
{
    MModePhaseMask binMask;
    bool ok = calcBinMask(mask, binMask);
    return ok && m_measModePhaseSetter->tryChangeMask(binMask);
}

QString MeasMode::getCurrentMask() const
{
    if(isValid())
        return MeasModeMaskStringConverter::calcStringMask(m_measModePhaseSetter->getCurrPhaseMask(), m_measSysCount);
    return "000";
}

bool MeasMode::isPhaseActive(int phase) const
{
    if(isValid() && phase >= 0 && phase<m_measSysCount) {
        MModePhaseMask mask = m_measModePhaseSetter->getCurrPhaseMask();
        return mask[m_measSysCount-phase-1];
    }
    return false;
}

int MeasMode::getMeasSysCount() const
{
    return m_measSysCount;
}

int MeasMode::getActiveMeasSysCount() const
{
    if(isValid())
        return m_measModePhaseSetter->getActiveMeasSysCount();
    return 0;
}

int MeasMode::getMaxMeasSysCount() const
{
    if(isValid())
        return m_measModePhaseSetter->getMaxMeasSysCount();
    return 0;
}

bool MeasMode::isValid() const
{
    return m_measSysCount > 0 &&
            m_measSysCount <= MeasPhaseCount &&
            m_measModeInfo.isValid();
}
