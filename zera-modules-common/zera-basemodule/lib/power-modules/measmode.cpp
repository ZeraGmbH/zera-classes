#include "measmode.h"
#include "measmodecatalog.h"

MeasMode::MeasMode(QString modeName, measmodes dspMode, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter)
{
    m_measModeStaticInfo = MeasModeCatalog::getInfo(modeName);
    m_dspMode = dspMode;
    m_measModePhaseSetter = std::move(measModePhaseSetter);
}

const cMeasModeInfo *MeasMode::getInfo()
{
    return m_measModeStaticInfo;
}

measmodes MeasMode::getDspMode()
{
    return m_dspMode;
}

void MeasMode::tryChangeMask(MModePhaseMask phaseMask)
{
    m_measModePhaseSetter->tryChangeMask(phaseMask);
    emit sigMaskTransactionFinished();
}

MModePhaseMask MeasMode::getCurrentMask()
{
    return m_measModePhaseSetter->getCurrPhaseMask();
}

