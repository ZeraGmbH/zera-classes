
#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>
#include <QString>

#include "scpiinterface.h"
#include "scpimeasuredelegate.h"
#include "scpiparameterdelegate.h"
#include "scpiinterfacedelegate.h"
#include "scpimeasure.h"
#include "scpicmdinfo.h"
#include "scpiclient.h"


namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(QString name)
    :m_sName(name)
{
    m_pSCPICmdInterface = new cSCPI(m_sName);

    cSCPIInterfaceDelegate* delegate;

    delegate = new cSCPIInterfaceDelegate(QString("DEVICE"), QString("IFACE"), 2, m_pSCPICmdInterface, deviceinterfacecmd);
    m_scpiInterfaceDelegateList.append(delegate);
    connect(delegate, SIGNAL(executeSCPI(cSCPIClient*, int, QString&)), this, SLOT(executeCmd(cSCPIClient*, int, QString&)));
}


cSCPIInterface::~cSCPIInterface()
{
    for (int i = 0; i < m_scpiInterfaceDelegateList.count(); i++ )
        delete m_scpiInterfaceDelegateList.at(i);
    for (int i = 0; i < m_scpiParameterDelegateList.count(); i++ )
        delete m_scpiParameterDelegateList.at(i);
    for (int i = 0; i < m_scpiMeasureDelegateHash.count(); i++ )
        delete m_scpiParameterDelegateList.at(i);

    delete m_pSCPICmdInterface;
}


void cSCPIInterface::addSCPICommand(cSCPICmdInfo &scpiCmdInfo) // here we add commands resulting from json documents from measure modules
{
    if (scpiCmdInfo.scpiModel == "MEASURE")
    {
        // in case of measure model we add several commands by ourself

        cSCPIMeasure* measureObject = new cSCPIMeasure(scpiCmdInfo.entity, scpiCmdInfo.scpiModuleName, scpiCmdInfo.cmdNode, scpiCmdInfo.unit);

        addSCPIMeasureCommand(QString(""), QString("MEASURE"), SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("CONFIGURE"), SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString(""), QString("READ"), SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString(""), QString("INIT"), SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString(""), QString("FETCH"), SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH"), scpiCmdInfo.scpiModuleName, SCPI::isNode | SCPI::isQuery, SCPIModelType::fetch, measureObject);

        addSCPIMeasureCommand(QString("MEASURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::measure, measureObject);
        addSCPIMeasureCommand(QString("CONFIGURE:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, SCPIModelType::configure, measureObject);
        addSCPIMeasureCommand(QString("READ:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::read, measureObject);
        addSCPIMeasureCommand(QString("INIT:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isCmd, SCPIModelType::init, measureObject);
        addSCPIMeasureCommand(QString("FETCH:%2").arg(scpiCmdInfo.scpiModuleName), scpiCmdInfo.cmdNode, SCPI::isQuery, SCPIModelType::fetch, measureObject);

    }
    else
    {
        bool ok;
        cSCPIParameterDelegate* delegate;

        QString cmdParent = QString("%1:%2").arg(scpiCmdInfo.scpiModel).arg(scpiCmdInfo.scpiModuleName);
        delegate = new cSCPIParameterDelegate(cmdParent, scpiCmdInfo.cmdNode, scpiCmdInfo.type.toInt(&ok), m_pSCPICmdInterface, scpiCmdInfo.peer, scpiCmdInfo.entity);
        m_scpiParameterDelegateList.append(delegate);
    }
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


void cSCPIInterface::addSCPIMeasureCommand(QString cmdparent, QString cmd, quint8 cmdType, quint8 measCode, cSCPIMeasure *measureObject)
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
        delegate = new cSCPIMeasureDelegate(cmdparent, cmd, cmdType , m_pSCPICmdInterface, measCode, measureObject);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
    }
}


void cSCPIInterface::executeCmd(cSCPIClient* client, int cmdCode, QString& sInput)
{
    cSCPICommand cmd = sInput;

    switch (cmdCode)
    {
    case deviceinterfacecmd:
    {
        if (cmd.isQuery())
        {
            QString xml;
            m_pSCPICmdInterface->exportSCPIModelXML(xml);
            client->receiveAnswer(xml);
        }
        else
            client->receiveAnswer(SCPI::scpiAnswer[SCPI::nak]);

        break;
    }

    }

}

}
