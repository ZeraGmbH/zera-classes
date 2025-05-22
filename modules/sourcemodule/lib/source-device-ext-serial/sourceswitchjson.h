#ifndef SOURCEINTERACTORSWITCH_H
#define SOURCEINTERACTORSWITCH_H

#include "abstractsourceswitchjson.h"
#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include "persistentjsonstate.h"

class SourceSwitchJson : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJson(ISourceIo::Ptr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch);
    void switchState(JsonParamApi paramState) override;
    void switchOff() override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onResponseReceived(const IoQueueGroup::Ptr transferGroup);
private:
    void handleSwitchResponse(const IoQueueGroup::Ptr transferGroup);
    ISourceIo::Ptr m_sourceIo;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    IdKeeperMulti m_pendingSwitchIds;
    PersistentJsonState* m_persistentParamState;
    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;
};

#endif // SOURCEINTERACTORSWITCH_H
