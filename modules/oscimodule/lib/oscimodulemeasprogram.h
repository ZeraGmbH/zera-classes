#ifndef OSCIMODULEMEASPROGRAM_H
#define OSCIMODULEMEASPROGRAM_H

#include "oscimodule.h"
#include "actualvaluestartstophandler.h"
#include "oscimoduleconfigdata.h"
#include <basedspmeasprogram.h>
#include <QFinalState>

namespace OSCIMODULE
{

class cOsciModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cOsciModuleMeasProgram(cOsciModule* module,
                           const std::shared_ptr<BaseModuleConfiguration> &configuration);
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
    cOsciModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void setActualValuesNames();
    void dataAcquisitionDSP();
    void dataReadDSP();

    cOsciModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pOsciCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pRefChannelParameter;

    DspVarGroupClientInterface* m_pParameterDSP;
    DspVarGroupClientInterface* m_pActualValuesDSP;

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
