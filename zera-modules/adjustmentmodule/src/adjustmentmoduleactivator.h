#ifndef ADJUSTMENTMODULEACTIVATOR_H
#define ADJUSTMENTMODULEACTIVATOR_H

#include "adjustmentmodulecommon.h"
#include "moduleactivist.h"
#include <QState>
#include <QFinalState>
#include <QStateMachine>
#include <QTimer>
#include <functional>

namespace ADJUSTMENTMODULE
{

class AdjustmentModuleActivator : public cModuleActivist
{
    Q_OBJECT
public:
    AdjustmentModuleActivator(AdjustmentServersAndConfig &moduleObjects, AdjustmentModuleActivateData &activationData);
    void setUpStateMachine();
signals:
private slots:
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
private:
    cAdjustmentModuleConfigData *getConfData();

    AdjustmentServersAndConfig &m_moduleAndServices;
    AdjustmentModuleActivateData &m_activationData;

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

    QHash<quint32, int> m_MsgNrCmdList;
    int activationIt;
    // timer for cyclic eeprom access enable (authorization) query
    QTimer m_AuthTimer;
};

}
#endif // ADJUSTMENTMODULEACTIVATOR_H
