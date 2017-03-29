#include <scpi.h>
#include <scpicommand.h>

#include "scpiinterface.h"
#include "scpimeasuredelegate.h"
#include "scpimeasurecollector.h"
#include "moduleinterface.h"
#include "scpimeasure.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIMeasureDelegate::cSCPIMeasureDelegate(QString cmdParent, QString cmd, quint8 type, quint8 measCode, cSCPIMeasure* scpimeasureobject)
    :cSCPIDelegate(cmdParent, cmd, type), m_nMeasCode(measCode)
{
    m_scpimeasureObjectList.append(scpimeasureobject);
    m_nPending = 0;
}


cSCPIMeasureDelegate::cSCPIMeasureDelegate(const cSCPIMeasureDelegate & delegate, QHash<cSCPIMeasure *, cSCPIMeasure *> &scpiMeasureTranslationHash)
{
    m_nMeasCode = delegate.m_nMeasCode;
    m_nPending = 0;
    for (int i = 0; i < delegate.m_scpimeasureObjectList.count(); i++)
    {
        cSCPIMeasure* scpiModuleMeasure = delegate.m_scpimeasureObjectList.at(i);
        if (scpiMeasureTranslationHash.contains(scpiModuleMeasure))
            m_scpimeasureObjectList.append(scpiMeasureTranslationHash[scpiModuleMeasure]);
        else
        {
            cSCPIMeasure* scpiMeasure = new cSCPIMeasure(*scpiModuleMeasure);
            scpiMeasureTranslationHash[scpiModuleMeasure] = scpiMeasure;
            m_scpimeasureObjectList.append(scpiMeasure);
        }
    }
}


cSCPIMeasureDelegate::~cSCPIMeasureDelegate()
{
    for  (int i = 0; i < m_scpimeasureObjectList.count(); i++)
        delete m_scpimeasureObjectList.at(i);
}


bool cSCPIMeasureDelegate::executeSCPI(cSCPIClient *client, QString &sInput)
{
    quint8 scpiCmdType;
    cSCPICommand cmd = sInput;

    scpiCmdType = getType();

    if ( (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) ||
         (cmd.isCommand(0) && ((scpiCmdType & SCPI::isCmd) > 0)) )
    {
        // allowed query or command
        return client->m_SCPIMeasureDelegateHash[this]->executeClient(client);
    }
    else
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), client, SLOT(receiveStatus(quint8)));
        emit signalStatus(SCPI::nak);
        disconnect(myConn);
    }

    return true;
}


bool cSCPIMeasureDelegate::executeClient(cSCPIClient *client)
{
    m_pClient = client;

    if (m_nPending == 0) // not yet running
    {
        m_nPending = m_scpimeasureObjectList.count();
        m_sAnswer = "";

        for (int i = 0; i < m_scpimeasureObjectList.count(); i++)
        {
            cSCPIMeasure* measure = m_scpimeasureObjectList.at(i);
            switch (m_nMeasCode)
            {
            case SCPIModelType::measure:
                connect(measure, SIGNAL(sigMeasDone(QString)), this, SLOT(receiveAnswer(QString)));
                break;
            case SCPIModelType::configure:
                connect(measure, SIGNAL(sigConfDone()), this, SLOT(receiveDone()));
                break;
            case SCPIModelType::read:
                connect(measure, SIGNAL(sigReadDone(QString)), this, SLOT(receiveAnswer(QString)));
                break;
            case SCPIModelType::init:
                connect(measure, SIGNAL(sigInitDone()), this, SLOT(receiveDone()));
                break;
            case SCPIModelType::fetch:
                connect(measure, SIGNAL(sigFetchDone(QString)), this, SLOT(receiveAnswer(QString)));
                break;
            }

            measure->execute(m_nMeasCode);
        }
    }
    else
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), client, SLOT(receiveStatus(quint8)));
        emit signalStatus(SCPI::nak);
        disconnect(myConn);
    }

    return true;
}


void cSCPIMeasureDelegate::addscpimeasureObject(cSCPIMeasure *measureobject)
{
    m_scpimeasureObjectList.append(measureobject);
}


void cSCPIMeasureDelegate::receiveDone()
{
    cSCPIMeasure* measure = qobject_cast<cSCPIMeasure*>(QObject::sender());
    disconnect(measure,0,this,0);
    m_nPending--;
    if (m_nPending == 0)
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalStatus(quint8)), m_pClient, SLOT(receiveStatus(quint8)));
        emit signalStatus(SCPI::ack);
        disconnect(myConn);
    }
}


void cSCPIMeasureDelegate::receiveAnswer(QString s)
{
    cSCPIMeasure* measure = qobject_cast<cSCPIMeasure*>(QObject::sender());
    disconnect(measure,0,this,0);
    m_sAnswer += QString("%1;").arg(s);
    m_nPending--;
    if (m_nPending == 0)
    {
        QMetaObject::Connection myConn = connect(this, SIGNAL(signalAnswer(QString)), m_pClient, SLOT(receiveAnswer(QString)));
        emit signalAnswer(m_sAnswer);
        disconnect(myConn);
    }
}

}
