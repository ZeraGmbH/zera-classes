#ifndef SOURCEINTERACTORSWITCH_H
#define SOURCEINTERACTORSWITCH_H

#include "sourcedevice.h"
#include "sourcetransactionstartnotifier.h"
#include "json/persistentjsonstate.h"
#include <QObject>

class SourceSwitchJson : public QObject
{
    Q_OBJECT
public:
    SourceSwitchJson(SourceDeviceInterface* sourceDevice, SourceTransactionStartNotifier *sourceNotificationSwitch);

    void switchState(JsonParamApi paramState);
    void switchOff();

    JsonParamApi getCurrLoadState();
signals:
    void sigSwitchFinished();

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onResponseReceived(const IoTransferDataGroup::Ptr transferGroup);
private:
    void handleSwitchResponse(const IoTransferDataGroup::Ptr transferGroup);

    SourceDeviceInterface* m_sourceDevice;
    SourceTransactionStartNotifier *m_sourceNotificationSwitch;

    IdKeeperMulti m_pendingSwitchIds;

    PersistentJsonState* m_persistentParamState;

    JsonParamApi m_paramsRequested;
    JsonParamApi m_paramsCurrent;

};

#endif // SOURCEINTERACTORSWITCH_H
