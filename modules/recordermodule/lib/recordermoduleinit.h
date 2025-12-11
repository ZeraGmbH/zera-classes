#ifndef RECORDERMODULEINIT_H
#define RECORDERMODULEINIT_H

#include "moduleactivist.h"
#include "recordermodule.h"

class RecorderModuleInit : public cModuleActivist
{
public:
    RecorderModuleInit(RecorderModule *module);
public slots:
    void activate() override;
    void deactivate() override;
    void generateVeinInterface() override;
private slots:
    void onActivateDone(bool ok);
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    RecorderModule *m_module;
    VfModuleComponent* m_numberOfPointsInCurve;
    VfModuleParameter* m_startStopRecording;
    VfModuleRpcPtr m_pGetRecordedValuesRpc;
};

#endif // RECORDERMODULEINIT_H
