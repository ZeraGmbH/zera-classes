
#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>
#include <QString>

#include "scpiinterface.h"
#include "scpiclient.h"
#include "scpibasedelegate.h"


namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(QString name)
    :m_sName(name)
{
    m_pSCPICmdInterface = new cSCPI();
}


cSCPIInterface::~cSCPIInterface()
{
    delete m_pSCPICmdInterface;
}

void cSCPIInterface::exportSCPIModelXML(QString &xml, QMap<QString, QString> modelListBaseEntry)
{
    modelListBaseEntry.insert(modelListBaseEntry.constBegin(), "DEVICE", m_sName);
    m_pSCPICmdInterface->exportSCPIModelXML(xml, modelListBaseEntry);
}

void cSCPIInterface::addSCPICommand(ScpiBaseDelegate *delegate)
{
    delegate->setCommand(m_pSCPICmdInterface);
}


bool cSCPIInterface::executeCmd(cSCPIClient *client, QString cmd)
{
    cSCPIObject* scpiObject;
    cmdInfos cmdInfo;
    cmdInfo.cmd = cmd;
    cmdInfo.client = client;
    if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0)
    {
        ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject);
        if(m_scpiCmdInExec.isEmpty()) {
            m_scpiCmdInExec.append(cmdInfo);
            connect(client, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
            return scpiDelegate->executeSCPI(client, cmd);
        }
        else {
            m_scpiCmdsWaiting.append(cmdInfo);
            return true;
        }
    }
    return false; // maybe that it is a common command
}

bool cSCPIInterface::checkAllCmds()
{
    if(!m_scpiCmdsWaiting.isEmpty()) {
        cSCPIObject* scpiObject;
        cmdInfos cmdInfo = m_scpiCmdsWaiting.takeFirst();
        cSCPIClient *clients = cmdInfo.client;
        QString cmds = cmdInfo.cmd;
        m_scpiCmdInExec.append(cmdInfo);
        if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmds)) != 0) {
            ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject);
            connect(clients, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
            return scpiDelegate->executeSCPI(clients, cmds);
        }
    }
    return false;
}

void cSCPIInterface::removeCommand()
{
    m_scpiCmdInExec.clear();
    checkAllCmds();
}


}
