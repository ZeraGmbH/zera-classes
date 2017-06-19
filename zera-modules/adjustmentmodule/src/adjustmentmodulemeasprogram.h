#ifndef ADJUSTMENTMODULEMEASPROGRAM_H
#define ADJUSTMENTMODULEMEASPROGRAM_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <pcbinterface.h>

#include "adjustmentmoduleconfigdata.h"
#include "basemeasworkprogram.h"
#include "measchannelinfo.h"
#include "measmodeinfo.h"
#include "foutinfo.h"

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
    sendrmident,
    readresourcetypes,
    readresource,
    readresourceinfo,
    readchnalias,
    readrangelist,
    adjustcomputation,
    adjuststorage,
    adjustinit,
    setadjuststatus,
    getgaincorrection,
    setgainnode,
    getphasecorrection,
    setphasenode,
    getoffsetcorrection,
    setoffsetnode
};


class cAdjustmentModuleConfigData;
class cAdjustmentModule;


class cAdjustChannelInfo
{
public:
    cAdjustChannelInfo();

    QString m_sAlias;
    QStringList m_sRangelist;
    Zera::Server::cPCBInterface* m_pPCBInterface;
};


class cAdjustmentModuleMeasProgram: public cBaseMeasWorkProgram
{
    Q_OBJECT

public:
    cAdjustmentModuleMeasProgram(cAdjustmentModule* module, Zera::Proxy::cProxy* proxy, cAdjustmentModuleConfigData& configdata);
    virtual ~cAdjustmentModuleMeasProgram();
    virtual void generateInterface(); // here we export our interface (entities)
    virtual void deleteInterface(); // we delete interface in case of reconfiguration

signals:
    void computationContinue();
    void computationDone();
    void storageContinue();
    void storageDone();
    void adjustamplitudeContinue();
    void adjustoffsetContinue();
    void adjustphaseContinue();
    void adjustError();


public slots:
    virtual void start(); // difference between start and stop is that actual values
    virtual void stop(); // in interface are not updated when stop

private:
    cAdjustmentModule* m_pModule;
    Zera::Proxy::cProxy* m_pProxy;
    cAdjustmentModuleConfigData& m_ConfigData;
    Zera::Server::cPCBInterface* m_AdjustPCBInterface; //we use the following 7 parameters globally defined for easier
    QString m_sAdjustSysName; // use within statemachines ... we have to keep in mind that adjustment
    QString m_sAdjustChannel; // commands can only be used in sequence not in parallel
    QString m_sAdjustRange;
    double m_AdjustTargetValue;
    double m_AdjustActualValue;
    double m_AdjustCorrection;
    int m_AdjustEntity;
    QString m_AdjustComponent;

    void setAdjustEnvironment(QVariant var);
    double cmpPhase(QVariant var);

    Zera::Server::cRMInterface* m_pRMInterface;
    Zera::Proxy::cProxyClient* m_pRMClient;

    QHash<QString,int> m_chnPortHash; // a hash for our channels ethernet ports
    QHash<int, QString> m_portChannelHash; // a list of ports for which we have established connection
    QHash<QString, QString> m_AliasChannelHash; // we use this hash for easier access
    QList<Zera::Proxy::cProxyClient*> m_pcbClientList; // a list with our pcb clients ... for clean up
    QList<Zera::Server::cPCBInterface*> m_pcbInterfaceList; // a list of pcbinterfaces ... for clean up
    QHash<QString, cAdjustChannelInfo*> m_adjustChannelInfoHash;
    QHash<quint32, int> m_MsgNrCmdList;
    int m_nAdjustGainIt;
    int m_nAdjustOffsetIt;
    int m_nAdjustPhaseIt;

    QList<cVeinModuleActvalue*> m_ActValueList; // the list of actual values we work on
    cVeinModuleMetaData* m_pHPWCountInfo; // the number of values we produce

    cVeinModuleParameter* m_pPARComputation;
    cVeinModuleParameter* m_pPARStorage;
    cVeinModuleParameter* m_pPARAdjustStatus;
    cVeinModuleParameter* m_pPARAdjustInit;
    cVeinModuleParameter* m_pPARAdjustAmplitude;
    cVeinModuleParameter* m_pPARAdjustPhase;
    cVeinModuleParameter* m_pPARAdjustOffset;

    // statemachine for activating gets the following states
    QState m_rmConnectState; // we must connect first to resource manager
    QState m_IdentifyState; // we must identify ourself at resource manager
    QState m_readResourceTypesState; // we ask for a list of all resources
    QState m_readResourceState; // we look for all our resources needed
    QState m_readResourceInfoState; // we look for resource specification
    QState m_readResourceInfoLoopState;
    QState m_pcbConnectionState; // we try to get a connection to all our pcb servers
    QState m_pcbConnectionLoopState;
    QState m_readChnAliasState; // we query all our channels alias
    QState m_readChnAliasLoopState;
    QState m_readRangelistState; // we query the range list for all our channels
    QState m_readRangelistLoopState;
    QState m_searchActualValuesState;
    QFinalState m_activationDoneState;

    int activationIt;

    // statemachine for deactivating
    QState m_deactivateState;
    QFinalState m_deactivateDoneState;

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

    // statemachine for amplitude adjustment
    QStateMachine m_adjustAmplitudeMachine;
    QState m_adjustamplitudeGetCorrState;
    QState m_adjustamplitudeSetNodeState;
    QFinalState m_adjustamplitudeFinishState;

    // statemachine for offset adjustment
    QStateMachine m_adjustOffsetMachine;
    QState m_adjustoffsetGetCorrState;
    QState m_adjustoffsetSetNodeState;
    QFinalState m_adjustoffsetFinishState;

    // statemachine for phase adjustment
    QStateMachine m_adjustPhaseMachine;
    QState m_adjustphaseGetCorrState;
    QState m_adjustphaseSetNodeState;
    QFinalState m_adjustphaseFinishState;

private slots:
    void setInterfaceValidation();

    void rmConnect();
    void sendRMIdent();
    void readResourceTypes();
    void readResource();
    void readResourceInfo();
    void readResourceInfoLoop();
    void pcbConnection();
    void pcbConnectionLoop();
    void readDspChannel();
    void readChnAlias();
    void readChnAliasLoop();
    void readRangelist();
    void readRangelistLoop();
    void searchActualValues();
    void activationDone();

    void deactivateMeas();
    void deactivateMeasDone();

    void computationStartCommand(QVariant var);
    void computationStart();
    void computationTest();
    void computationFinished();

    void storageStartCommand(QVariant var);
    void storageStart();
    void storageTest();
    void storageFinished();

    void setAdjustStatusStartCommand(QVariant var);
    void setAdjustInitStartCommand(QVariant var);

    void setAdjustAmplitudeStartCommand(QVariant var);
    void adjustamplitudeGetCorr();
    void adjustamplitudeSetNode();

    void setAdjustPhaseStartCommand(QVariant var);
    void adjustphaseGetCorr();
    void adjustphaseSetNode();

    void setAdjustOffsetStartCommand(QVariant var);
    void adjustoffsetGetCorr();
    void adjustoffsetSetNode();

    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

};

}
#endif // ADJUSTMENTMODULEMEASPROGRAM_H
