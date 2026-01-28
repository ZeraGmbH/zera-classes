#ifndef RECORDERMODULEINIT_H
#define RECORDERMODULEINIT_H

#include "moduleactivist.h"
#include "recordermodule.h"
#include "recordermoduleconfigdata.h"
#include "veindatacollector.h"
#include <timersingleshotqt.h>

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
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    QHash<int, QStringList> setEntitiesAndComponentsToRecord();
    void startStopLogging(QVariant startStopRecording);
    void setNumberOfPointsInCurve(int num);

    RecorderModule *m_module;
    RecorderModuleConfigData* m_confData;
    std::shared_ptr<VeinDataCollector> m_dataCollector;

    TimerTemplateQtPtr m_stopLoggingTimer;
    VfModuleComponent* m_numberOfPointsInCurve;
    VfModuleParameter* m_startStopRecording;
    VfModuleRpcPtr m_pReadRecordedValuesRpc;
};

#endif // RECORDERMODULEINIT_H
