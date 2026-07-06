#include "scpimeasurescpicmdnodedelegate.h"
#include "perveincomponentmeasuretransaction.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE {

ScpiMeasureScpiCmdNodeDelegate::ScpiMeasureScpiCmdNodeDelegate(const QString &cmdParent,
                                           const QString &cmd,
                                           quint8 scpiCmdQueryFlags,
                                           ScpiModelTypes modelType,
                                           PerVeinComponentMeasureTransaction* scpimeasureobject) :
    ScpiBaseDelegate(cmdParent, cmd, scpiCmdQueryFlags),
    m_modelType(modelType)
{
    m_scpimeasureObjectList.append(scpimeasureobject);
}

ScpiMeasureScpiCmdNodeDelegate::ScpiMeasureScpiCmdNodeDelegate(const ScpiMeasureScpiCmdNodeDelegate &moduleInterfaceDelegate,
                                           QHash<PerVeinComponentMeasureTransaction*, PerVeinComponentMeasureTransaction*> &scpiMeasureTranslationHash) :
    m_modelType(moduleInterfaceDelegate.m_modelType)
{
    for (int i = 0; i < moduleInterfaceDelegate.m_scpimeasureObjectList.count(); i++) {
        PerVeinComponentMeasureTransaction* scpiModuleMeasure = moduleInterfaceDelegate.m_scpimeasureObjectList.at(i);
        if (scpiMeasureTranslationHash.contains(scpiModuleMeasure))
            m_scpimeasureObjectList.append(scpiMeasureTranslationHash[scpiModuleMeasure]);
        else {
            // Until not fixed properly: scpiMeasureTranslationHash is in cSCPIClient and
            // that makes it the owner of the PerVeinComponentMeasureTransaction-object generated here
            // => no delete in here but in ~cSCPIClient
            PerVeinComponentMeasureTransaction* scpiMeasure = new PerVeinComponentMeasureTransaction(*scpiModuleMeasure);
            scpiMeasureTranslationHash[scpiModuleMeasure] = scpiMeasure;
            m_scpimeasureObjectList.append(scpiMeasure);
        }
    }
}

void ScpiMeasureScpiCmdNodeDelegate::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    quint8 scpiCmdType = getType();
    if ( (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) ||
         (cmd.isCommand(0) && ((scpiCmdType & SCPI::isCmd) > 0)) ) {
        // allowed query or command
        client->m_SCPIMeasureDelegateHash[this]->executeClient(client, scpiTransactionId);
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiMeasureScpiCmdNodeDelegate::executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId)
{
    bool reentryPossible;
    switch (m_modelType)
    {
    case ScpiModelTypes::init:
    case ScpiModelTypes::configure:
        reentryPossible = true;
        break;
    default:
        reentryPossible = false;
    }
    m_pClient = client;
    if (m_nPending == 0 || reentryPossible) { // not yet running or reentry
        m_nPending = m_scpimeasureObjectList.count();
        m_sAnswer = "";
        for (int i = 0; i < m_scpimeasureObjectList.count(); i++) {
            PerVeinComponentMeasureTransaction* measure = m_scpimeasureObjectList.at(i);
            switch (m_modelType)
            {
            case ScpiModelTypes::measure:
                connect(measure, &PerVeinComponentMeasureTransaction::sigMeasDone, this, &ScpiMeasureScpiCmdNodeDelegate::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::configure:
                connect(measure, &PerVeinComponentMeasureTransaction::sigConfDone, this, &ScpiMeasureScpiCmdNodeDelegate::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::read:
                connect(measure, &PerVeinComponentMeasureTransaction::sigReadDone, this, &ScpiMeasureScpiCmdNodeDelegate::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::init:
                connect(measure, &PerVeinComponentMeasureTransaction::sigInitDone, this, &ScpiMeasureScpiCmdNodeDelegate::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::fetch:
                connect(measure, &PerVeinComponentMeasureTransaction::sigFetchDone, this, &ScpiMeasureScpiCmdNodeDelegate::onSingleScpiQueryDone);
                break;
            }
            measure->execute(m_modelType, scpiTransactionId);
        }
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiMeasureScpiCmdNodeDelegate::addScpiMeasureObject(PerVeinComponentMeasureTransaction *measureobject)
{
    m_scpimeasureObjectList.append(measureobject);
}

void ScpiMeasureScpiCmdNodeDelegate::onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const PerVeinComponentMeasureTransaction *sender)
{
    disconnect(sender, 0, this, 0);

    m_nPending--;
    if (m_nPending == 0)
        m_pClient->handleCmdFinishStatusOnly(ZSCPI::ack, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiCmdDone: m_nPending < 0");
}

void ScpiMeasureScpiCmdNodeDelegate::onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const PerVeinComponentMeasureTransaction *sender)
{
    disconnect(sender, 0, this, 0);

    m_sAnswer += QString("%1;").arg(scpiResponse);

    m_nPending--;
    if (m_nPending == 0)
        m_pClient->handleCmdFinish(m_sAnswer, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiQueryDone: m_nPending < 0");
}

}
