#ifndef THDNMODULEMEASPROGRAM_H
#define THDNMODULEMEASPROGRAM_H

#include "basedspmeasprogram.h"
#include "measchannelinfo.h"
#include "movingwindowfilter.h"
#include "actualvaluestartstophandler.h"
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <timerperiodicqt.h>


class cDspMeasData;
class QStateMachine;
class QState;
class QFinalState;

class VfModuleComponent;
class VfModuleParameter;
class VfModuleMetaData;
class VfModuleComponent;
class cMovingwindowFilter;


namespace THDNMODULE
{

enum thdnmoduleCmds
{
    varlist2dsp,
    cmdlist2dsp,
    activatedsp,
    deactivatedsp,
    dataaquistion,
    writeparameter,
};

#define irqNr 4

class cThdnModuleConfigData;
class cThdnModule;

class cThdnModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cThdnModuleMeasProgram(cThdnModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    void start() override;
    void stop() override;
protected slots:
    virtual void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cThdnModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();

    cThdnModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    VfModuleMetaData* m_pThdnCountInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationTimeParameter;

    cDspMeasData* m_pTmpDataDsp;
    cDspMeasData* m_pParameterDSP;
    cDspMeasData* m_pActualValuesDSP;

    // statemachine for activating gets the following states
    QState m_dspserverConnectState;
    QState m_var2DSPState;
    QState m_cmd2DSPState;
    QState m_activateDSPState;
    QFinalState m_loadDSPDoneState;

    // statemachine for deactivating
    QFinalState m_unloadDSPDoneState;

    // statemachine for reading actual values
    QStateMachine m_dataAcquisitionMachine;
    QState m_dataAcquisitionState;
    QFinalState m_dataAcquisitionDoneState;

    void setActualValuesNames();
    void setSCPIMeasInfo();

    cMovingwindowFilter m_movingwindowFilter;

private slots:
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPdone();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newIntegrationtime(QVariant ti);
};

}
#endif // THDNMODULEMEASPROGRAM_H
