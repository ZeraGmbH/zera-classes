#include "measmode.h"
#include "measmodecatalog.h"

MeasMode::MeasMode(QString modeName, measmodes dspMode, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter)
{
    m_measModeInfo = MeasModeCatalog::getInfo(modeName);
    m_dspMode = dspMode;
    m_measModePhaseSetter = std::move(measModePhaseSetter);
}

cMeasModeInfo MeasMode::getInfo() const
{
    return m_measModeInfo;
}

measmodes MeasMode::getDspMode()
{
    return m_dspMode;
}

void MeasMode::tryChangeMask(MModePhaseMask phaseMask)
{
    if(m_measModePhaseSetter->tryChangeMask(phaseMask))
        emit sigMaskChanged();
    else
        emit sigMaskChangeFailed();
}

MModePhaseMask MeasMode::getCurrentMask()
{
    return m_measModePhaseSetter->getCurrPhaseMask();
}

