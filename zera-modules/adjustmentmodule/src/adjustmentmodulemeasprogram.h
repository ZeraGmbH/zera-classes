#ifndef ADJUSTMENTMODULEMEASPROGRAM_H
#define ADJUSTMENTMODULEMEASPROGRAM_H

#include "adjustmentmoduleconfigdata.h"
#include "adjustmentmodulecommon.h"
#include "adjustmentmoduleactivator.h"
#include "rminterface.h"
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"
#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QTimer>
#include <QFinalState>

namespace Zera {
namespace Proxy {
    class cProxy;
}
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}

class cBaseModule;
class cVeinModuleActvalue;
class cVeinModuleMetaData;
class cVeinModuleComponent;
class cVeinModuleParameter;


namespace ADJUSTMENTMODULE
{

enum adjustmentmoduleCmds
{
    adjustcomputation,
    adjuststorage,
    setadjustgainstatus,
    setadjustphasestatus,
    setadjustoffsetstatus,
    getadjgaincorrection,
    setgainnode,
    getadjphasecorrection,
    setphasenode,
    enGetAdjOffsetCorrection,
    enGetAdjOffsetRejection,
    enGetAdjOffsetRejectionValue,
    setoffsetnode,
    getpcbadjustmentdata,
    setpcbadjustmentdata,
    getclampadjustmentdata,
    setclampadjustmentdata,
    getauthorizationstatus,
    sendtransparentcmd,
};


class cAdjustmentModuleConfigData;
class cAdjustmentModule;


class cAdjustChannelInfo
{
public:
    cAdjustChannelInfo(){}

    QString m_sAlias;
    QStringList m_sRangelist;
    Zera::Server::cPCBInterface* m_pPCBInterface;
};


class cAdjustIterators
{
public:
    cAdjustIterators();

    int m_nAdjustGainIt;
    int m_nAdjustOffsetIt;
    int m_nAdjustPhaseIt;
};


class cAdjustmentModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cAdjustmentModuleMeasProgram(cAdjustmentModule* module, Zera::Proxy::cProxy* proxy, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    void generateInterface() override; // here we export our interface (entities)
    bool isAuthorized();

signals:
    void computationContinue();
    void computationDone();
    void storageContinue();
    void storageDone();
    void adjustamplitudeContinue();
    void adjustoffsetContinue();
    void adjustphaseContinue();
    void adjustError();
    void activationReady();

public slots:
    void start()  override; // difference between start and stop is that actual values
    void stop() override; // in interface are not updated when stop
    void activate() override;
    void deactivate() override;

private:
    cAdjustmentModuleConfigData* getConfData();
    bool checkExternalVeinComponents();
    void setInterfaceValidation();
    cAdjustmentModule* m_pModule;
    Zera::Server::cPCBInterface* m_currAdjustPCBInterface;
    // we use the following 9 parameters globally defined for easier
    // use within statemachines ... we have to keep in mind that adjustment
    // commands can only be used in sequence not in parallel
    QString m_sAdjustSysName;
    QString m_sAdjustChannel;
    QString m_sAdjustRange;
    double m_AdjustTargetValue;
    double m_AdjustActualValue;
    double m_AdjustFrequency;
    double m_AdjustCorrection;
    double m_AdjustRejection;
    double m_AdjustRejectionValue;

    int m_AdjustEntity;
    QString m_AdjustComponent;
    bool m_bAuthorized;
    QVariant receivedPar;

    void setAdjustEnvironment(QVariant var);
    double cmpPhase(QVariant var);
    double symAngle(double ang);

    Zera::Proxy::cProxyClient* m_pRMClient;

    AdjustmentModuleActivateDataPtr m_commonActivationObjects;
    AdjustmentModuleActivator m_activator;
    QHash<quint32, int> m_MsgNrCmdList;
    QHash<QString, cAdjustIterators*> m_adjustIteratorHash;
    int m_nAdjustGainIt;
    int m_nAdjustOffsetIt;
    int m_nAdjustPhaseIt;

    cVeinModuleParameter* m_pPARComputation;
    cVeinModuleParameter* m_pPARStorage;
    cVeinModuleParameter* m_pPARAdjustGainStatus;
    cVeinModuleParameter* m_pPARAdjustPhaseStatus;
    cVeinModuleParameter* m_pPARAdjustOffsetStatus;
    cVeinModuleParameter* m_pPARAdjustInit;
    cVeinModuleParameter* m_pPARAdjustAmplitude;
    cVeinModuleParameter* m_pPARAdjustPhase;
    cVeinModuleParameter* m_pPARAdjustOffset;
    cVeinModuleParameter* m_pPARAdjustSend;
    cVeinModuleParameter* m_pPARAdjustPCBData;
    cVeinModuleParameter* m_pPARAdjustClampData;

    // statemachine for computation of adjustment values
    QStateMachine m_computationMachine;
    QState m_computationStartState;
    QState m_computationTestState;
    QFinalState m_computationFinishState;
    int computationIt;

    // statemachine for storage of adjustment values
    QStateMachine m_storageMachine;
    QState m_storageStartState;
    QState m_storageTestState;
    QFinalState m_storageFinishState;
    int storageIt;
    int storageType;

    // statemachine for amplitude adjustment
    QStateMachine m_adjustAmplitudeMachine;
    QState m_adjustamplitudeGetCorrState;
    QState m_adjustamplitudeSetNodeState;
    QFinalState m_adjustamplitudeFinishState;

    // statemachine for offset adjustment
    QStateMachine m_adjustOffsetMachine;
    QState m_adjustOffsetGetCorrState;
    QState m_adjustOffsetGetRejection;
    QState m_adjustOffsetGetRejectionValue;
    QState m_adjustoffsetSetNodeState;
    QFinalState m_adjustoffsetFinishState;

    // statemachine for phase adjustment
    QStateMachine m_adjustPhaseMachine;
    QState m_adjustphaseGetCorrState;
    QState m_adjustphaseSetNodeState;
    QFinalState m_adjustphaseFinishState;

    // timer for cyclic eeprom access enable (authorization) query
    QTimer m_AuthTimer;

private slots:
    void onActivationReady();
    void onDeactivationReady();

    void computationStartCommand(QVariant var);
    void computationStart();
    void computationTest();
    void computationFinished();

    void storageStartCommand(QVariant var);
    void storageStart();
    void storageTest();
    void storageFinished();

    void setAdjustGainStatusStartCommand(QVariant var);
    void setAdjustPhaseStatusStartCommand(QVariant var);
    void setAdjustOffsetStatusStartCommand(QVariant var);
    void setAdjustInitStartCommand(QVariant var);

    void setAdjustAmplitudeStartCommand(QVariant var);
    void adjustamplitudeGetCorr();
    void adjustamplitudeSetNode();

    void setAdjustPhaseStartCommand(QVariant var);
    void adjustphaseGetCorr();
    void adjustphaseSetNode();

    void setAdjustOffsetStartCommand(QVariant var);

    void transparentDataSend2Port(QVariant var);
    void writePCBAdjustmentData(QVariant var);
    void readPCBAdjustmentData(QVariant);
    void writeCLAMPAdjustmentData(QVariant var);
    void readCLAMPAdjustmentData(QVariant);

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

};

}
#endif // ADJUSTMENTMODULEMEASPROGRAM_H
