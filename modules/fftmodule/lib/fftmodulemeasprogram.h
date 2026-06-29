#ifndef FFTMODULEMEASPROGRAM_H
#define FFTMODULEMEASPROGRAM_H

#include "fftmodule.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <movingwindowfilter.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace FFTMODULE
{

class cFftModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    explicit cFftModuleMeasProgram(cFftModule* module);
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

    void newIntegrationtime(QVariant ti);
    void newRefChannel(QVariant chn);
private:
    void setDspVarList();
    void setDspCmdList();
    void dataAcquisitionDSP();
    void dataReadDSP();
    quint16 calcFftResultLenHalf(quint8 fftOrder);

    cFftModule* m_pModule = nullptr;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleComponent*> m_DCValueList;
    VfModuleMetaData* m_pFFTCountInfo = nullptr;
    VfModuleMetaData* m_pFFTOrderInfo = nullptr;
    VfModuleComponent* m_pMeasureSignal = nullptr;
    VfModuleParameter* m_pRefChannelParameter = nullptr;
    VfModuleParameter* m_pIntegrationTimeParameter = nullptr;
    quint16 m_nfftLen = 0;

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

    MovingwindowFilter m_movingwindowFilter;
    QVector<float> m_FFTModuleActualValues;
};

}
#endif // FFTMODULEMEASPROGRAM_H
