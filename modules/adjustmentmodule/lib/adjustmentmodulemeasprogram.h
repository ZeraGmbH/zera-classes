#ifndef ADJUSTMENTMODULEMEASPROGRAM_H
#define ADJUSTMENTMODULEMEASPROGRAM_H

#include "adjustmentmoduleconfigdata.h"
#include "adjustmentmodulecommon.h"
#include "adjustmentmoduleactivator.h"
#include <rminterface.h>
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "foutinfo.h"
#include "taskcontainersequence.h"
#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

namespace Zera {
namespace  Server {
    class cRMInterface;
    class cPCBInterface;
}
}

class cBaseModule;
class VfModuleActvalue;
class VfModuleMetaData;
class VfModuleComponent;
class VfModuleParameter;


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
    setoffsetnode,
    getpcbadjustmentdata,
    setpcbadjustmentdata,
    getclampadjustmentdata,
    setclampadjustmentdata,
    sendtransparentcmd,
};


class cAdjustmentModuleConfigData;
class cAdjustmentModule;


class cAdjustIterators
{
public:
    cAdjustIterators();

    int m_nAdjustGainIt;
    int m_nAdjustPhaseIt;
};

constexpr double maxAmplitudeErrorPercent = 10;
constexpr double maxPhaseErrorDegrees = 3;

class cAdjustmentModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cAdjustmentModuleMeasProgram(cAdjustmentModule* module, std::shared_ptr<cBaseModuleConfiguration> pConfiguration);
    void generateInterface() override; // here we export our interface (entities)

signals:
    void computationContinue();
    void storageContinue();
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
    void openPcbConnection();
    bool checkExternalVeinComponents();
    void setInterfaceValidation();
    void setAdjustEnvironment(QVariant var);
    double cmpPhase(QVariant var);
    double symAngle(double ang);
    double calcAdjAbsoluteError();

    cAdjustmentModule* m_pModule;
    // we use the following 7 parameters globally defined for easier
    // use within statemachines ... we have to keep in mind that adjustment
    // commands can only be used in sequence not in parallel
    QString m_sAdjustSysName;
    QString m_sAdjustChannel;
    QString m_sAdjustRange;
    double m_AdjustTargetValue;
    double m_AdjustActualValue;
    double m_AdjustFrequency;
    double m_AdjustCorrection;

    QVariant m_receivedPar;

    AdjustmentModuleCommonPtr m_commonObjects;
    AdjustmentModuleActivator m_activator;
    QHash<quint32, int> m_MsgNrCmdList;
    QHash<QString, cAdjustIterators*> m_adjustIteratorHash;

    VfModuleParameter* m_pPARComputation;
    VfModuleParameter* m_pPARStorage;
    VfModuleParameter* m_pPARAdjustGainStatus;
    VfModuleParameter* m_pPARAdjustPhaseStatus;
    VfModuleParameter* m_pPARAdjustOffsetStatus;
    VfModuleParameter* m_pPARAdjustInit;
    VfModuleParameter* m_pPARAdjustAmplitude;
    VfModuleParameter* m_pPARAdjustPhase;
    VfModuleParameter* m_pPARAdjustOffset;
    VfModuleParameter* m_pPARAdjustSend;
    VfModuleParameter* m_pPARAdjustPCBData;
    VfModuleParameter* m_pPARAdjustClampData;

    // statemachine for computation of adjustment values
    QStateMachine m_computationMachine;
    QState m_computationStartState;
    QFinalState m_computationFinishState;

    // statemachine for storage of adjustment values
    QStateMachine m_storageMachine;
    QState m_storageStartState;
    QFinalState m_storageFinishState;
    int storageType;

    // statemachine for amplitude adjustment
    QStateMachine m_adjustAmplitudeMachine;
    QState m_adjustamplitudeGetCorrState;
    QState m_adjustamplitudeSetNodeState;
    QFinalState m_adjustamplitudeFinishState;

    TaskContainerParallel m_offsetTasks;

    // statemachine for phase adjustment
    QStateMachine m_adjustPhaseMachine;
    QState m_adjustphaseGetCorrState;
    QState m_adjustphaseSetNodeState;
    QFinalState m_adjustphaseFinishState;
private slots:
    void onActivationReady();
    void onDeactivationReady();
    void onNewRanges();

    void computationStartCommand(QVariant var);
    void computationStart();
    void computationFinished();

    void storageStartCommand(QVariant var);
    void storageStart();
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

#endif // ADJUSTMENTMODULEMEASPROGRAM_H
