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

void cSCPIInterface::addSCPICommand(ScpiBaseDelegatePtr delegate)
{
    delegate->setCommand(m_pSCPICmdInterface, delegate);
}

void cSCPIInterface::waitForBlockingCmd(cSCPIClient *client)
{
    m_expCmd = TimerFactoryQt::createSingleShot(5000);
    connect(m_expCmd.get(), &TimerTemplateQt::sigExpired, this, [this, client]{cSCPIInterface::removeCommand(client);});
    m_expCmd->start();
}

bool cSCPIInterface::executeCmd(cSCPIClient *client, QString cmd)
{
    ScpiObjectPtr scpiObject;
    cmdInfos cmdInfo;
    cmdInfo.cmd = cmd;
    cmdInfo.client = client;
    if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0)
    {
        ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject.get());
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

void cSCPIInterface::checkAmbiguousShortNames()
{
    ScpiAmbiguityMap ambiguityMap = m_pSCPICmdInterface->checkAmbiguousShortNames(ignoreAmbiguous);
    if(!ambiguityMap.isEmpty()) {
        for(auto iter=ambiguityMap.constBegin(); iter!=ambiguityMap.constEnd(); ++iter) {
            qWarning("SCPI-module: Ambiguous short %s for %s",
                     qPrintable(iter.key()),
                     qPrintable(iter.value().join(" / ")));
        }
        Q_ASSERT(false);
    }
}

ScpiAmbiguityMap cSCPIInterface::ignoreAmbiguous(ScpiAmbiguityMap inMap)
{
    ScpiAmbiguityMap outMap;

    for(auto iter = inMap.constBegin(); iter != inMap.constEnd(); ++iter) {
        bool ignore = false;

        static QStringList ignoreEndShortList = QStringList() <<
                                                ":DFT1:UL3-" <<
                                                ":RMS1:UL3-" <<
                                                ":LAM1:LAMB" <<
                                                ":EC01:0001:RES" <<
                                                ":EC01:0002:RES" <<
                                                "CONF:EC01:0001:DUTC" <<
                                                "CONF:EC01:0002:DUTC" <<
                                                ":FFT1:IAUX" <<
                                                ":FFT1:UAUX" <<
                                                "STAT:DEV1:ACC";
        for(const auto &ignoreEndShort : qAsConst(ignoreEndShortList)) {
            if(iter.key().endsWith(ignoreEndShort))
                ignore = true;
        }

        if(!ignore)
            outMap[iter.key()] = iter.value();
    }

    return outMap;
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

void cSCPIInterface::checkAllCmds()
{
    if(!m_scpiCmdInExec.isEmpty()) {
         ScpiObjectPtr scpiObject;
         cmdInfos cmdInfo = m_scpiCmdInExec.head();
         cSCPIClient *client = cmdInfo.client;
         QString cmd = cmdInfo.cmd;
         if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0) {
             ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject.get());
             if(m_enableScpiQueue) {
                 connect(client, &cSCPIClient::commandAnswered, this, &cSCPIInterface::removeCommand);
                 waitForBlockingCmd(client);
                 scpiDelegate->executeSCPI(client, cmd);
             }
             else {
                 scpiDelegate->executeSCPI(client, cmd);
                 while(!m_scpiCmdInExec.isEmpty()) {
                     cmdInfos cmdInfo = m_scpiCmdInExec.dequeue();
                     cSCPIClient *client = cmdInfo.client;
                     QString cmd = cmdInfo.cmd;
                     if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0) {
                         ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject.get());
                         scpiDelegate->executeSCPI(client, cmd);
                     }
                 }
             }
         }
     }
}
}
