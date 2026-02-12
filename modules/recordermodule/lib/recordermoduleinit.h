#ifndef RECORDERMODULEINIT_H
#define RECORDERMODULEINIT_H

#include "moduleactivist.h"
#include "recordermodule.h"
#include "recordermoduleconfigdata.h"
#include "vfmodulecomponentstoragefetchonly.h"
#include <timersingleshotqt.h>
#include <vs_storagerecorder.h>
#include <storagerecorderinterpolation.h>

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

    TimerTemplateQtPtr m_stopLoggingTimer; // Hack: avoid out of memory by stopping after a while

    std::shared_ptr<VeinStorage::StorageRecorder> m_recorder;
    std::unique_ptr<StorageRecorderInterpolation> m_recorderInterpolation;
    VfModuleComponentStorageFetchOnly* m_jsonExportComponent = nullptr;
    VfModuleParameter* m_numberOfPointsInCurve = nullptr;
    VfModuleParameter* m_startStopRecording = nullptr;
    VfModuleRpcPtr m_pReadRecordedValuesRpc = nullptr;
    VfModuleRpcPtr m_pGetRecordedSampleValuesRpc = nullptr;
};

#endif // RECORDERMODULEINIT_H
