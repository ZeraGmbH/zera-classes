#ifndef SOURCEINTERACTORSWITCH_H
#define SOURCEINTERACTORSWITCH_H

#include "sourceio.h"
#include "sourcetransactionstartnotifier.h"
#include "persistentjsonstate.h"
#include <QObject>

class SourceSwitchJson : public QObject
{
    Q_OBJECT
public:
    SourceSwitchJson(ISourceIo::Ptr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch);
    void switchState(JsonParamApi paramState);
    void switchOff();
    JsonParamApi getCurrLoadState();
    JsonParamApi getRequestedLoadState();
signals:
    void sigSwitchFinished();

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
