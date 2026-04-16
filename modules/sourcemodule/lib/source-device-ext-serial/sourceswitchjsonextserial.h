#ifndef SOURCEINTERACTORSWITCH_H
#define SOURCEINTERACTORSWITCH_H

#include "abstractsourceswitchjson.h"
#include "idkeeper.h"
#include "iogroupgeneratorextserial.h"
#include "sourcetransactionstartnotifier.h"
#include "persistentjsonstate.h"

class SourceSwitchJsonExtSerial : public AbstractSourceSwitchJson
{
    Q_OBJECT
public:
    SourceSwitchJsonExtSerial(AbstractSourceIoPtr sourceIo, SourceTransactionStartNotifier::Ptr sourceNotificationSwitch);
    void switchState(JsonParamApi paramState) override;
    void switchOff() override;
    JsonParamApi getCurrLoadState() override;
    JsonParamApi getRequestedLoadState() override;

private slots:
    void onSwitchTransactionStarted(int dataGroupId);
    void onResponseReceived(const IoQueueGroup::Ptr transferGroup);
private:
    void handleSwitchResponse(const IoQueueGroup::Ptr transferGroup);
    AbstractSourceIoPtr m_sourceIo;
    IoGroupGeneratorExtSerial m_ioGroupGenerator;
    SourceTransactionStartNotifier::Ptr m_sourceNotificationSwitch;
    IdKeeperMulti m_pendingSwitchIds;
    PersistentJsonState m_persistentParamState;
    JsonParamApi m_paramsCurrent;
    JsonParamApi m_paramsRequested;
};

#endif // SOURCEINTERACTORSWITCH_H
