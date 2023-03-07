#include "measmode.h"
#include "measmodecatalog.h"

MeasMode::MeasMode(QString modeName, int dspSelectCode, int measSysCount, MeasModePhaseSetStrategyPtr measModePhaseSetter) :
    m_measModeInfo(MeasModeCatalog::getInfo(modeName)),
    m_dspSelectCode(dspSelectCode),
    m_measModePhaseSetter(std::move(measModePhaseSetter)),
    m_measSysCount(measSysCount),
    m_bValid(true)
{
    if(measSysCount < 1 || measSysCount > MeasPhaseCount)
        m_bValid = false;
}

cMeasModeInfo MeasMode::getInfo() const
{
    return m_measModeInfo;
}

int MeasMode::getDspSelectCode()
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

QString MeasMode::getCurrentMask()
{
    MModePhaseMask binMask = m_measModePhaseSetter->getCurrPhaseMask();
    QString mask;
    for(int i=std::min(MeasPhaseCount, m_measSysCount)-1; i>=0; i--)
        mask += binMask[i] ? "1" : "0";
    return mask;
}

bool MeasMode::isPhaseActive(int phase)
{
    if(phase >= 0 && phase<m_measSysCount) {
        MModePhaseMask mask = m_measModePhaseSetter->getCurrPhaseMask();
        return mask[m_measSysCount-phase-1];
    }
    return false;
}

int MeasMode::getActiveMeasSysCount()
{
    return m_measModePhaseSetter->getActiveMeasSysCount();
}

bool MeasMode::isValid()
{
    return m_bValid && m_measModeInfo.isValid();
}
