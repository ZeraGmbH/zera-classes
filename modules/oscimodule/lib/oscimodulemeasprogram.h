#ifndef OSCIMODULEMEASPROGRAM_H
#define OSCIMODULEMEASPROGRAM_H

#include "oscimodule.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <QFinalState>

namespace OSCIMODULE
{

class cOsciModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    explicit cOsciModuleMeasProgram(cOsciModule* module);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void newRefChannel(const QVariant &chn);
private:
    void setDspVarList();
    void setDspCmdList();
    void setActualValuesNames();
    void dataAcquisitionDSP();
    void dataReadDSP();

    cOsciModule* m_pModule = nullptr;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pOsciCountInfo = nullptr;
    VfModuleComponent* m_pMeasureSignal = nullptr;
    VfModuleParameter* m_pRefChannelParameter = nullptr;

    DspVarGroupClientInterface* m_pParameterDSP = nullptr;
    DspVarGroupClientInterface* m_pActualValuesDSP = nullptr;

    // statemachine for activating gets the following states
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QState m_unloadStart;
    QFinalState m_unloadDSPDoneState;

    TaskTemplatePtr m_taskDataAcquisition;
};

}
#endif // OSCIMODULEMEASPROGRAM_H
