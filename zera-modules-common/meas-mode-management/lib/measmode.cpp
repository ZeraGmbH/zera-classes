#include "measmode.h"
#include "measmodecatalog.h"

MeasMode::MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter) :
    m_measModeInfo(MeasModeCatalog::getInfo(modeName)),
    m_dspSelectCode(dspSelectCode),
    m_measModePhaseSetter(std::move(measModePhaseSetter)),
    m_measSysCount(measSysCount)
{
}

QString MeasMode::getName() const
{
    return m_measModeInfo.getName();
}

int MeasMode::getDspSelectCode() const
{
    return m_dspSelectCode;
}

bool MeasMode::tryChangeMask(QString mask)
{
    bool ok = isValid() && mask.size() == m_measSysCount;
    MModePhaseMask binMask;
    if(ok) {
        for(int i=0; i<m_measSysCount; i++) {
            int strMaskEntry = m_measSysCount-i-1;
            if(mask[strMaskEntry] == '0')
                binMask[i] = 0;
            else if(mask[strMaskEntry] == '1')
                binMask[i] = 1;
            else {
                ok = false;
                break;
            }
        }
    }
    return ok && m_measModePhaseSetter->tryChangeMask(binMask);
}

QString MeasMode::getCurrentMask() const
{
    if(isValid()) {
        QString mask;
        MModePhaseMask binMask = m_measModePhaseSetter->getCurrPhaseMask();
        for(int i=std::min(MeasPhaseCount, m_measSysCount)-1; i>=0; i--)
            mask += binMask[i] ? "1" : "0";
        return mask;
    }
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

int MeasMode::getActiveMeasSysCount() const
{
    if(isValid())
        return m_measModePhaseSetter->getActiveMeasSysCount();
    else
        return 0;
}

bool MeasMode::isValid() const
{
    return m_measSysCount > 0 &&
            m_measSysCount <= MeasPhaseCount &&
            m_measModeInfo.isValid();
}
