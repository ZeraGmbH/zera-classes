#include "scpidelegatemeasuremodule.h"
#include "veincomponentscpimeasuresequence.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE {

ScpiDelegateMeasureModule::ScpiDelegateMeasureModule(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiQueryCmdFlags),
    m_modelType(params.modelType)
{
    m_veinComponentScpiSequences.append(params.scpiMeasureObject);
}

ScpiDelegateMeasureModule::ScpiDelegateMeasureModule(const ScpiDelegateMeasureModule &moduleInterfaceDelegate,
                                           QHash<VeinComponentScpiMeasureSequence*, VeinComponentScpiMeasureSequence*> &scpiMeasureTranslationHash) :
    m_modelType(moduleInterfaceDelegate.m_modelType)
{
    for (int i = 0; i < moduleInterfaceDelegate.m_veinComponentScpiSequences.count(); i++) {
        VeinComponentScpiMeasureSequence* scpiModuleMeasure = moduleInterfaceDelegate.m_veinComponentScpiSequences.at(i);
        if (scpiMeasureTranslationHash.contains(scpiModuleMeasure))
            m_veinComponentScpiSequences.append(scpiMeasureTranslationHash[scpiModuleMeasure]);
        else {
            // Until not fixed properly: scpiMeasureTranslationHash is in cSCPIClient and
            // that makes it the owner of the VeinComponentScpiMeasureSequence-object generated here
            // => no delete in here but in ~cSCPIClient
            VeinComponentScpiMeasureSequence* scpiMeasure = new VeinComponentScpiMeasureSequence(*scpiModuleMeasure);
            scpiMeasureTranslationHash[scpiModuleMeasure] = scpiMeasure;
            m_veinComponentScpiSequences.append(scpiMeasure);
        }
    }
}

void ScpiDelegateMeasureModule::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    quint8 scpiQueryCmdFlags = getType();
    if ( (cmd.isQuery() && ((scpiQueryCmdFlags & SCPI::isQuery) > 0)) ||
         (cmd.isCommand(0) && ((scpiQueryCmdFlags & SCPI::isCmd) > 0)) ) {
        // allowed query or command
        client->m_SCPIMeasureDelegateHash[this]->executeClient(client, scpiTransactionId);
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiDelegateMeasureModule::executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId)
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
    m_client = client;
    if (m_nPending == 0 || reentryPossible) { // not yet running or reentry
        m_nPending = m_veinComponentScpiSequences.count();
        m_sAnswer = "";
        for (int i = 0; i < m_veinComponentScpiSequences.count(); i++) {
            VeinComponentScpiMeasureSequence* measure = m_veinComponentScpiSequences.at(i);
            switch (m_modelType)
            {
            case ScpiModelTypes::measure:
                connect(measure, &VeinComponentScpiMeasureSequence::sigMeasDone, this, &ScpiDelegateMeasureModule::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::configure:
                connect(measure, &VeinComponentScpiMeasureSequence::sigConfDone, this, &ScpiDelegateMeasureModule::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::read:
                connect(measure, &VeinComponentScpiMeasureSequence::sigReadDone, this, &ScpiDelegateMeasureModule::onSingleScpiQueryDone);
                break;
            case ScpiModelTypes::init:
                connect(measure, &VeinComponentScpiMeasureSequence::sigInitDone, this, &ScpiDelegateMeasureModule::onSingleScpiCmdDone);
                break;
            case ScpiModelTypes::fetch:
                connect(measure, &VeinComponentScpiMeasureSequence::sigFetchDone, this, &ScpiDelegateMeasureModule::onSingleScpiQueryDone);
                break;
            }
            measure->execute(m_modelType, scpiTransactionId);
        }
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void ScpiDelegateMeasureModule::addVeinComponentScpiSequence(VeinComponentScpiMeasureSequence *measureobject)
{
    m_veinComponentScpiSequences.append(measureobject);
}

void ScpiDelegateMeasureModule::onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId, const VeinComponentScpiMeasureSequence *sender)
{
    disconnect(sender, 0, this, 0);

    m_nPending--;
    if (m_nPending == 0)
        m_client->handleCmdFinishStatusOnly(ZSCPI::ack, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiCmdDone: m_nPending < 0");
}

void ScpiDelegateMeasureModule::onSingleScpiQueryDone(const QString &scpiResponse, const ScpiTransactionId &scpiTransactionId, const VeinComponentScpiMeasureSequence *sender)
{
    disconnect(sender, 0, this, 0);

    m_sAnswer += QString("%1;").arg(scpiResponse);

    m_nPending--;
    if (m_nPending == 0)
        m_client->handleCmdFinish(m_sAnswer, scpiTransactionId);
    else if (m_nPending < 0)
        qCritical("cSCPIMeasureDelegate::onSingleScpiQueryDone: m_nPending < 0");
}

}
