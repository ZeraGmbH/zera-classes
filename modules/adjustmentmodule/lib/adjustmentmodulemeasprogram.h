#ifndef ADJUSTMENTMODULEMEASPROGRAM_H
#define ADJUSTMENTMODULEMEASPROGRAM_H

#include "adjustmentmoduleconfigdata.h"
#include "adjustmentmodulecommon.h"
#include "adjustmentmoduleactivator.h"
#include "basemeasworkprogram.h"
#include "taskcontainerparallel.h"
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

class BaseModule;
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

constexpr double maxAmplitudeErrorPercent = 5;
constexpr double maxPhaseErrorDegrees = 3;

class cAdjustmentModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cAdjustmentModuleMeasProgram(cAdjustmentModule* module, std::shared_ptr<BaseModuleConfiguration> pConfiguration);
    void generateVeinInterface() override;
    static double symAngle(double ang);

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
private:
    cAdjustmentModuleConfigData* getConfData();
    bool checkExternalVeinComponents();
    void setInterfaceValidation();
    void setAdjustEnvironment(QVariant var);
    double cmpPhase(QVariant var);
    double calcAbsoluteError(double actualValue, double targetValue);
    double calcAdjAbsoluteError();
    bool checkRangeIsWanted(QString adjType);

    cAdjustmentModule* m_pModule;
    // we use the following 7 parameters globally defined for easier
    // use within statemachines ... we have to keep in mind that adjustment
    // commands can only be used in sequence not in parallel
    struct TCurrentAdjEnvironment {
        QString m_channelMName;
        QString m_channelAlias;
        QString m_rangeName;
        QVariant m_receivedPar;
        double m_targetValue = 0.0;
        double m_actualValue = 0.0;
        double m_AdjustFrequency = 0.0;
        double m_AdjustCorrection = 0.0;
    } m_currEnv;

    AdjustmentModuleCommonPtr m_commonObjects;
    AdjustmentModuleActivator m_activator;
    QHash<quint32, int> m_MsgNrCmdList;
    QHash<QString, cAdjustIterators*> m_adjustIteratorHash;

    VfModuleParameter* m_pPARComputation = nullptr;
    VfModuleParameter* m_pPARStorage = nullptr;
    VfModuleParameter* m_pPARAdjustGainStatus = nullptr;
    VfModuleParameter* m_pPARAdjustPhaseStatus = nullptr;
    VfModuleParameter* m_pPARAdjustOffsetStatus = nullptr;
    VfModuleParameter* m_pPARAdjustInit = nullptr;
    VfModuleParameter* m_pPARAdjustAmplitude = nullptr;
    VfModuleParameter* m_pPARAdjustPhase = nullptr;
    VfModuleParameter* m_pPARAdjustOffset = nullptr;
    VfModuleParameter* m_pPARAdjustSend = nullptr;
    VfModuleParameter* m_pPARAdjustPCBData = nullptr;
    VfModuleParameter* m_pPARAdjustClampData = nullptr;

    // statemachine for computation of adjustment values
    QStateMachine m_computationMachine;
    QState m_computationStartState;
    QFinalState m_computationFinishState;

    // statemachine for storage of adjustment values
    QStateMachine m_storageMachine;
    QState m_storageStartState;
    QFinalState m_storageFinishState;
    enum StorageType {
        UNDEFINED = 0,
        INTERNAL = 1,
        CLAMP = 2
    };
    StorageType m_storageType = UNDEFINED;

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

};

#endif // ADJUSTMENTMODULEMEASPROGRAM_H
