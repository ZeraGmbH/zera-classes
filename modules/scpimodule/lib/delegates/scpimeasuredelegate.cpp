#include "scpimeasuredelegate.h"
#include "moduleinterface.h"
#include "scpimeasure.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE {

cSCPIMeasureDelegate::cSCPIMeasureDelegate(const QString &cmdParent,
                                           const QString &cmd,
                                           quint8 type,
                                           quint8 measCode,
                                           cSCPIMeasure* scpimeasureobject) :
    ScpiBaseDelegate(cmdParent, cmd, type),
    m_nMeasCode(measCode)
{
    m_scpimeasureObjectList.append(scpimeasureobject);
}

cSCPIMeasureDelegate::cSCPIMeasureDelegate(const cSCPIMeasureDelegate &delegate, QHash<cSCPIMeasure*, cSCPIMeasure*> &scpiMeasureTranslationHash) :
    m_nMeasCode(delegate.m_nMeasCode)
{
    for (int i = 0; i < delegate.m_scpimeasureObjectList.count(); i++) {
        cSCPIMeasure* scpiModuleMeasure = delegate.m_scpimeasureObjectList.at(i);
        if (scpiMeasureTranslationHash.contains(scpiModuleMeasure))
            m_scpimeasureObjectList.append(scpiMeasureTranslationHash[scpiModuleMeasure]);
        else {
            // Until not fixed properly: scpiMeasureTranslationHash is in cSCPIClient and
            // that makes it the owner of the cSCPIMeasure-object generated here
            // => no delete in here but in ~cSCPIClient
            cSCPIMeasure* scpiMeasure = new cSCPIMeasure(*scpiModuleMeasure);
            scpiMeasureTranslationHash[scpiModuleMeasure] = scpiMeasure;
            m_scpimeasureObjectList.append(scpiMeasure);
        }
    }
}

void cSCPIMeasureDelegate::executeSCPI(cSCPIClient *client, const QString &scpi, const ScpiTransactionId &scpiTransactionId)
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

void cSCPIMeasureDelegate::executeClient(cSCPIClient *client, const ScpiTransactionId &scpiTransactionId)
{
    bool reentryPossible;
    switch (m_nMeasCode)
    {
    case SCPIModelType::init:
    case SCPIModelType::configure:
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
            cSCPIMeasure* measure = m_scpimeasureObjectList.at(i);
            switch (m_nMeasCode)
            {
            case SCPIModelType::measure:
                connect(measure, &cSCPIMeasure::sigMeasDone, this, &cSCPIMeasureDelegate::onSingleScpiQueryDone);
                break;
            case SCPIModelType::configure:
                connect(measure, &cSCPIMeasure::sigConfDone, this, &cSCPIMeasureDelegate::onSingleScpiCmdDone);
                break;
            case SCPIModelType::read:
                connect(measure, &cSCPIMeasure::sigReadDone, this, &cSCPIMeasureDelegate::onSingleScpiQueryDone);
                break;
            case SCPIModelType::init:
                connect(measure, &cSCPIMeasure::sigInitDone, this, &cSCPIMeasureDelegate::onSingleScpiCmdDone);
                break;
            case SCPIModelType::fetch:
                connect(measure, &cSCPIMeasure::sigFetchDone, this, &cSCPIMeasureDelegate::onSingleScpiQueryDone);
                break;
            }
            measure->execute(m_nMeasCode, scpiTransactionId);
        }
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

void cSCPIMeasureDelegate::addScpiMeasureObject(cSCPIMeasure *measureobject)
{
    m_scpimeasureObjectList.append(measureobject);
}

void cSCPIMeasureDelegate::onSingleScpiCmdDone(const ScpiTransactionId &scpiTransactionId)
{
    const cSCPIMeasure* measure = qobject_cast<cSCPIMeasure*>(QObject::sender());
    disconnect(measure,0,this,0);
    m_nPending--;
    if (m_nPending == 0)
        m_pClient->handleCmdFinishStatusOnly(ZSCPI::ack, scpiTransactionId);
}

void cSCPIMeasureDelegate::onSingleScpiQueryDone(QString s, const ScpiTransactionId &scpiTransactionId)
{
    const cSCPIMeasure* measure = qobject_cast<cSCPIMeasure*>(QObject::sender());
    disconnect(measure,0,this,0);
    m_sAnswer += QString("%1;").arg(s);
    m_nPending--;
    if (m_nPending == 0)
        m_pClient->handleCmdFinish(m_sAnswer, scpiTransactionId);
}

}
