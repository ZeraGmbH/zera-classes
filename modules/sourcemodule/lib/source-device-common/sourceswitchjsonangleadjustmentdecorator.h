#ifndef SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H
#define SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H

#include "abstractsourceswitchjson.h"
#include <vs_abstractcomponent.h>

class SourceSwitchJsonAngleAdjustmentDecorator : public AbstractSourceSwitchJson
{
public:
    explicit SourceSwitchJsonAngleAdjustmentDecorator(AbstractSourceSwitchJsonPtr switcherToAdjust,
                                                      const QJsonObject &sourceCapabilities,
                                                      const QList<VeinStorage::AbstractComponentPtr> &dftPolarComponentsU,
                                                      const QList<VeinStorage::AbstractComponentPtr> &dftPolarComponentsI,
                                                      VeinStorage::AbstractComponentPtr dftSigMeasuring);
    int switchState(const JsonParamApi &paramState) override;
    JsonParamApi getCurrLoadpoint() override;

private slots:
    void onSwitchFinishedSwitcherToAdjust(bool ok, int switchId);
    void onNewDftValues();
private:
    AbstractSourceSwitchJsonPtr m_switcherToAdjust;
    JsonParamApi m_targetLoadpointCurrent;

    const QList<VeinStorage::AbstractComponentPtr> m_dftPolarComponentsU;
    const QList<VeinStorage::AbstractComponentPtr> m_dftPolarComponentsI;
    VeinStorage::AbstractComponentPtr m_dftSigMeasuring;

    QHash<int, JsonParamApi> m_pendingExternalSwitchTransactions;
};

#endif // SOURCESWITCHJSONANGLEADJUSTMENTDECORATOR_H
