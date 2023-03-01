#include "measmode.h"
#include "measmodecatalog.h"

MeasMode::MeasMode(QString modeName, int dspSelectCode, std::unique_ptr<MeasModePhaseSetStrategy> measModePhaseSetter) :
    m_measModeInfo(MeasModeCatalog::getInfo(modeName)),
    m_dspSelectCode(dspSelectCode),
    m_measModePhaseSetter(std::move(measModePhaseSetter))
{
}

cMeasModeInfo MeasMode::getInfo() const
{
    return m_measModeInfo;
}

int MeasMode::getDspSelectCode()
{
    return m_dspSelectCode;
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
