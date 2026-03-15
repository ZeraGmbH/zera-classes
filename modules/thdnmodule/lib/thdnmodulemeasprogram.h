#ifndef THDNMODULEMEASPROGRAM_H
#define THDNMODULEMEASPROGRAM_H

#include "thdnmodule.h"
#include "thdnmoduleconfiguration.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <movingwindowfilter.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace THDNMODULE
{

class cThdnModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cThdnModuleMeasProgram(cThdnModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void newIntegrationtime(QVariant ti);
private:
    cThdnModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void dataAcquisitionDSP();
    void dataReadDSP();
    void setActualValuesNames();
    void setSCPIMeasInfo();

    cThdnModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pThdnCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationTimeParameter;

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

    cMovingwindowFilter m_movingwindowFilter;

};

}
#endif // THDNMODULEMEASPROGRAM_H
