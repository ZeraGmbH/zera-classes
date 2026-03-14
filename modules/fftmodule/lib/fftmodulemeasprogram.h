#ifndef FFTMODULEMEASPROGRAM_H
#define FFTMODULEMEASPROGRAM_H

#include "fftmodule.h"
#include "fftmoduleconfigdata.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <measchannelinfo.h>
#include <movingwindowfilter.h>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <timerperiodicqt.h>

namespace FFTMODULE
{

class cFftModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cFftModuleMeasProgram(cFftModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
public slots:
    virtual void start() override;
    virtual void stop() override;

private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void setInterfaceActualValues(QVector<float> *actualValues);

    void dspserverConnect();
    void varList2DSP();
    void cmdList2DSP();
    void activateDSP();
    void activateDSPdone();

    void deactivateDSPStart();

    void dataAcquisitionDSP();
    void dataReadDSP();

    void newIntegrationtime(QVariant ti);
    void newRefChannel(QVariant chn);
private:
    cFftModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    quint16 calcFftResultLenHalf(quint8 fftOrder);
    void setSCPIMeasInfo();
    void setActualValuesNames();

    cFftModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleComponent*> m_DCValueList;
    VfModuleMetaData* m_pFFTCountInfo;
    VfModuleMetaData* m_pFFTOrderInfo;
    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pRefChannelParameter;
    VfModuleParameter* m_pIntegrationTimeParameter;
    quint16 m_nfftLen;

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

    cMovingwindowFilter m_movingwindowFilter;
    QVector<float> m_FFTModuleActualValues;
};

}
#endif // FFTMODULEMEASPROGRAM_H
