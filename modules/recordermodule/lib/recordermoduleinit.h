#ifndef RECORDERMODULEINIT_H
#define RECORDERMODULEINIT_H

#include "moduleactivist.h"
#include "recordermodule.h"
#include "recordermoduleconfigdata.h"
#include <timersingleshotqt.h>
#include <vs_storagerecorder.h>

class RecorderModuleInit : public cModuleActivist
{
public:
    RecorderModuleInit(RecorderModule *module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
public slots:
    void activate() override;
    void deactivate() override;
    void generateVeinInterface() override;
private slots:
    void onActivateDone(bool ok);
    void startStopLogging(QVariant startStopRecording);
private:
    void createRecorder();

    RecorderModule *m_module;
    RecorderModuleConfigData* m_confData;
    std::unique_ptr<VeinStorage::StorageRecorder> m_recorder;

    TimerTemplateQtPtr m_stopLoggingTimer;
    VfModuleComponent* m_numberOfPointsInCurve;
    VfModuleParameter* m_startStopRecording;
    VfModuleRpcPtr m_pReadRecordedValuesRpc;
};

#endif // RECORDERMODULEINIT_H
