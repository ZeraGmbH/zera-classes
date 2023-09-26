
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
    return false; // maybe that it is a common command}
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


void cSCPIInterface::removeCommand(cSCPIClient *client)
{
    client->disconnect(this);
    m_scpiCmdInExec.clear();
    checkAllCmds();
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
}
