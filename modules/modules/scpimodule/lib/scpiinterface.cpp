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
        m_scpiCmdInExec.enqueue(cmdInfo);
        if(m_scpiCmdInExec.count() == 1) { // Before the list was empty so wen need to trigger the execution machinery
            connect(client, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
            return scpiDelegate->executeSCPI(client, cmd);
        }
    }
    return false; // maybe that it is a common command
}

void cSCPIInterface::removeCommand(cSCPIClient *client)
{
    client->disconnect(this);
    m_scpiCmdInExec.dequeue();
    checkAllCmds();
}

bool cSCPIInterface::checkAllCmds()
{
    if(!m_scpiCmdInExec.isEmpty()) {
         cSCPIObject* scpiObject;
         cmdInfos cmdInfo = m_scpiCmdInExec.head();
         cSCPIClient *client = cmdInfo.client;
         QString cmd = cmdInfo.cmd;
         if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0) {
             ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject);
             connect(client, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
             return scpiDelegate->executeSCPI(client, cmd);
         }
     }
     return false;
}
}
