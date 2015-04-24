#include <veinentity.h>
#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>
#include <QString>

#include "scpiinterface.h"
#include "scpinonmeasuredelegate.h"
#include "scpimeasure.h"
#include "scpimeasurecollector.h"
#include "scpimeasuredelegate.h"
#include "scpicmdinfo.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(QString name)
    :m_sName(name)
{
    m_pSCPICmdInterface = new cSCPI(m_sName);

    cSCPInonMeasureDelegate* delegate;
    // special case command with id 0 returns the interface as xml
    delegate = new cSCPInonMeasureDelegate(QString("DEVICE"), QString("IFACE"), 2, m_pSCPICmdInterface, 0, (VeinPeer*)0, (VeinEntity*)0);
    m_scpinonMeasureDelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));

    m_nCmdCode = 8; //
}


cSCPIInterface::~cSCPIInterface()
{
    delete m_pSCPICmdInterface;
}


void cSCPIInterface::addSCPICommand(cSCPICmdInfo &scpiCmdInfo)
{
    if (scpiCmdInfo.scpiModel == "MEASURE")
    {
        // in case of measure model we add several commands by ourself

        cSCPIMeasure* measureObject = new cSCPIMeasure(scpiCmdInfo.entity, scpiCmdInfo.scpiModuleName, scpiCmdInfo.cmdNode, scpiCmdInfo.unit);

        addSCPIMeasureCommand(QString(""), QString("MEASURE"), SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("CONFIGURE"), SCPI::isNode | SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("READ"), SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString(""), QString("INIT"), SCPI::isNode | SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString(""), QString("FETCH"), SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);

        m_nCmdCode += 8;

        addSCPIMeasureCommand(QString("MEASURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);

        m_nCmdCode += 8;

        addSCPIMeasureCommand(QString("MEASURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, m_nCmdCode + SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, m_nCmdCode + SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, m_nCmdCode + SCPIModelType::fetch, measureObject);


    }
    else
    {
        bool ok;
        cSCPInonMeasureDelegate* delegate;

        QString cmdParent = QString("%1:%2").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName);
        delegate = new cSCPInonMeasureDelegate(cmdParent, scpiCmdInfo.cmdNode, scpiCmdInfo.type.toInt(&ok), m_pSCPICmdInterface, m_nCmdCode + SCPIModelType::normal, scpiCmdInfo.peer, scpiCmdInfo.entity);
        m_scpinonMeasureDelegateList.append(delegate);
        connect(delegate, SIGNAL(execute(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));
    }

    m_nCmdCode+=8; // next command code

}


bool cSCPIInterface::executeCmd(cSCPIClient *client, QString cmd)
{
    cSCPIObject* scpiObject;
    QString dummy;

    if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd, dummy)) != 0)
    {
        cSCPIDelegate* scpiDelegate = static_cast<cSCPIDelegate*>(scpiObject);
        return scpiDelegate->executeSCPI(client, cmd);
    }

    return false; // maybe that it is a common command
}


void cSCPIInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint16 cmdCode, cSCPIMeasure *measureObject)
{
    cSCPIMeasureDelegate* delegate;
    QString cmdcomplete = QString("%1:%2").arg(cmdparent).arg(cmd);

    if (m_scpiMeasureDelegateHash.contains(cmdcomplete))
    {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addscpimeasureObject(measureObject);
    }
    else
    {
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , m_pSCPICmdInterface, cmdCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        connect(delegate, SIGNAL(execute(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int,QString&)));
    }
}


void cSCPIInterface::executeCmd(cSCPIClient* client, int cmdCode, QString& sInput)
{
    quint8 cmdMeas, scpiCmdType;
    bool validation;

    cSCPICommand cmd = sInput;

    cmdMeas = cmdCode % 8;

    if (cmdCode == 0)
    {
        QString xml;
        m_pSCPICmdInterface->exportSCPIModelXML(xml);
        client->receiveAnswer(xml);
    }

    else

    if (cmdMeas == 0) // so we have a non measure command
    {
        cSCPInonMeasureDelegate* delegate = qobject_cast<cSCPInonMeasureDelegate*>(sender());
        scpiCmdType = delegate->getType();
        VeinEntity* entity = delegate->getEntity();

        if (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) // test if we got an allowed query
        {
            QString answer = entity->getValue().toString();
            client->receiveAnswer(answer);
        }
        else

            if (cmd.isCommand(1) && ((scpiCmdType & SCPI::isCmdwP) > 0)) // test if we got an allowed cmd + 1 parameter
            {
                if (validation = true) // todo here
                {
                    QVariant var = entity->getValue();
                    QVariant newVar = cmd.getParam(0);
                    newVar.convert(var.type());
                    entity->setValue(newVar, delegate->getPeer());
                    client->receiveStatus(SCPI::ack);
                }
                else
                    client->receiveStatus(SCPI::errval);

            }

            else
                client->receiveStatus(SCPI::nak);
    }

    else // otherwise we have to measure, configure, read, init or fetch something
    {
        cSCPIMeasureDelegate* delegate = qobject_cast<cSCPIMeasureDelegate*>(sender());
        scpiCmdType = delegate->getType();

        if ( (cmd.isQuery() && ((scpiCmdType & SCPI::isQuery) > 0)) ||
             (cmd.isCommand(0) && ((scpiCmdType & SCPI::isCmd) > 0)) )
        {
            QList<cSCPIMeasure*>* measureObjectList = delegate->getscpimeasureObjectList();

            cSCPIMeasureCollector* measureCollector = new cSCPIMeasureCollector(client, measureObjectList->count());

            for (int i = 0; i < measureObjectList->count(); i++)
            {
                cSCPIMeasure* measure = measureObjectList->at(i);
                connect(measure, SIGNAL(cmdAnswer(QString)), measureCollector, SLOT(receiveAnswer(QString)));
                connect(measure, SIGNAL(cmdStatus(quint8)), measureCollector, SLOT(receiveStatus(quint8)));
                measure->execute(cmdMeas);
            }
        }
        else
            client->receiveStatus(SCPI::nak);
    }

}

}
