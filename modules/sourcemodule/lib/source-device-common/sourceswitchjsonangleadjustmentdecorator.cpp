#include "sourceswitchjsonangleadjustmentdecorator.h"

SourceSwitchJsonAngleAdjustmentDecorator::SourceSwitchJsonAngleAdjustmentDecorator(AbstractSourceSwitchJsonPtr switcherToAdjust,
                                                                                   const QJsonObject &sourceCapabilities,
                                                                                   const QList<VeinStorage::AbstractComponentPtr> &dftPolarComponentsU,
                                                                                   const QList<VeinStorage::AbstractComponentPtr> &dftPolarComponentsI,
                                                                                   VeinStorage::AbstractComponentPtr dftSigMeasuring) :
    m_switcherToAdjust(std::move(switcherToAdjust)),
    m_targetLoadpointCurrent(m_switcherToAdjust->getCurrLoadpoint()),
    m_dftPolarComponentsU(dftPolarComponentsU),
    m_dftPolarComponentsI(dftPolarComponentsI),
    m_dftSigMeasuring(dftSigMeasuring)
{
    connect(m_switcherToAdjust.get(), &AbstractSourceSwitchJson::sigSwitchFinished,
            this, &SourceSwitchJsonAngleAdjustmentDecorator::onSwitchFinishedSwitcherToAdjust);
    connect(m_dftSigMeasuring.get(), &VeinStorage::AbstractComponent::sigValueChange, this, [=](const QVariant &value) {
        if (value == 1)
            onNewDftValues();
    });
}

int SourceSwitchJsonAngleAdjustmentDecorator::switchState(const JsonParamApi &paramState)
{
    int switchId = m_switcherToAdjust->switchState(paramState);
    m_pendingExternalSwitchTransactions.insert(switchId, paramState);
    return switchId;
}

JsonParamApi SourceSwitchJsonAngleAdjustmentDecorator::getCurrLoadpoint()
{
    return m_targetLoadpointCurrent;
}

void SourceSwitchJsonAngleAdjustmentDecorator::onSwitchFinishedSwitcherToAdjust(bool ok, int switchId)
{
    // for now ignore responses on regulation (our transactions)
    if (m_pendingExternalSwitchTransactions.contains(switchId)) {
        JsonParamApi loadpointRequested = m_pendingExternalSwitchTransactions.take(switchId);
        if (ok)
            m_targetLoadpointCurrent = loadpointRequested;
        emit sigSwitchFinished(ok, switchId);
    }
}

void SourceSwitchJsonAngleAdjustmentDecorator::onNewDftValues()
{
}
