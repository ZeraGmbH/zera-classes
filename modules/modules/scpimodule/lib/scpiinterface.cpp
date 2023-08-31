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

void cSCPIInterface::waitForBlockingCmd(cSCPIClient *client)
{
    m_expCmd = TimerFactoryQt::createSingleShot(5000);
    connect(m_expCmd.get(), &TimerTemplateQt::sigExpired, this, [this, client]{cSCPIInterface::removeCommand(client);});
    m_expCmd->start();
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
        if(m_enableScpiQueue) {
            m_scpiCmdInExec.enqueue(cmdInfo);
            if(m_scpiCmdInExec.count() == 1) { // The list was empty before, so we need to trigger the execution machinery
                connect(client, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
                waitForBlockingCmd(client);
                scpiDelegate->executeSCPI(client, cmd);
            }
        }
        else
            scpiDelegate->executeSCPI(client, cmd);

        return true;
    }
    return false;
}

void cSCPIInterface::setEnableQueue(bool enable)
{
    m_enableScpiQueue = enable;
}

void cSCPIInterface::removeCommand(cSCPIClient *client)
{
    client->disconnect(this);
    m_expCmd->disconnect(this);
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
             waitForBlockingCmd(client);
             scpiDelegate->executeSCPI(client, cmd);
             return true;
         }
     }
     return false;
}
}
