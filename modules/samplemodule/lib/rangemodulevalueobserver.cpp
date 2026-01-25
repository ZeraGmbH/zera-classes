#include "rangemodulevalueobserver.h"

RangeModuleValueObserver::RangeModuleValueObserver(VeinStorage::AbstractDatabase *veinStorageDb) :
    m_veinStorageDb(veinStorageDb),
    m_rangeMeasSignalComponent(veinStorageDb->findComponent(RangeModuleEntityId, "SIG_Measuring"))
{
    connect(m_rangeMeasSignalComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
            this, &RangeModuleValueObserver::onRangeSigMeasChange);
}

float RangeModuleValueObserver::getRelativeRangeValue(const QString &channelMName) const
{
    if (!channelMName.startsWith("m"))
        return qQNaN();

    const QString channelIdxZeroBased = channelMName.right(channelMName.size()-1);
    const int idxOneBased = channelIdxZeroBased.toInt() + 1;

    const QString actRejComponentName = QString("INF_Channel%1ActREJ").arg(idxOneBased);
    const VeinStorage::AbstractComponentPtr actRejComponent = m_veinStorageDb->findComponent(RangeModuleEntityId, actRejComponentName);
    if (actRejComponent == nullptr)
        return qQNaN();
    const float rejValue = actRejComponent->getValue().toFloat();
    if (isRejValueForClampMissingOnClampOnlyChannel(rejValue))
        return 0;

    const QString actRmsComponentName = QString("ACT_Channel%1Rms").arg(idxOneBased);
    const VeinStorage::AbstractComponentPtr actRmsComponent = m_veinStorageDb->findComponent(RangeModuleEntityId, actRmsComponentName);
    if (actRmsComponent == nullptr)
        return qQNaN();

    return actRmsComponent->getValue().toFloat() / rejValue;
}

void RangeModuleValueObserver::onRangeSigMeasChange(QVariant newValue)
{
    if(newValue == 1)
        emit sigNewValues();
}

bool RangeModuleValueObserver::isRejValueForClampMissingOnClampOnlyChannel(float rejValue)
{
    return rejValue < 1e-10;
}
