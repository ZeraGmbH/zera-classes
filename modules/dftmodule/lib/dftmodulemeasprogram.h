#ifndef DFTMODULEMEASPROGRAM_H
#define DFTMODULEMEASPROGRAM_H

#include "dftmodule.h"
#include "dftmoduleconfiguration.h"
#include "actualvaluestartstophandler.h"
#include <basedspmeasprogram.h>
#include <movingwindowfilter.h>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace DFTMODULE
{

class cDftModuleMeasProgram: public cBaseDspMeasProgram
{
    Q_OBJECT
public:
    cDftModuleMeasProgram(cDftModule* module,
                          const std::shared_ptr<BaseModuleConfiguration> &configuration);
    void generateVeinInterface() override;
public slots:
    virtual void start() override; // difference between start and stop is that actual values
    virtual void stop() override; // in interface are not updated when stop

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
    void newRefChannel(QVariant refchn);
private:
    cDftModuleConfigData* getConfData();
    void setDspVarList();
    void setDspCmdList();
    void turnVectorsToRefChannel();
    void dataAcquisitionDSP();
    void dataReadDSP();
    void setActualValuesNames();
    void setSCPIMeasInfo();
    void setRefChannelValidator();
    void initRFieldMeasurement();
    bool isConfiguredForDcRef();

    cDftModule* m_pModule;
    ActualValueStartStopHandler m_startStopHandler;
    QList<VfModuleComponent*> m_veinActValueList; // the list of actual values we work on
    QList<VfModuleComponent*> m_veinPolarValue;
    VfModuleComponent* m_pRFieldActualValue;
    QHash<QString, QString> m_ChannelSystemNameHash; // a hash for fast access to the system name with key = alias
    QList<int> m_rfieldActvalueIndexList; // an index list for access to configured actual values

    VfModuleComponent* m_pMeasureSignal;
    VfModuleParameter* m_pIntegrationTimeParameter;
    VfModuleParameter* m_pRefChannelParameter;
    VfModuleMetaData* m_pDFTPNCountInfo;
    VfModuleMetaData* m_pDFTPPCountInfo;
    VfModuleMetaData* m_pDFTOrderInfo;

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

    MovingwindowFilter m_movingwindowFilter;
};

}
#endif // DFTMODULEMEASPROGRAM_H
