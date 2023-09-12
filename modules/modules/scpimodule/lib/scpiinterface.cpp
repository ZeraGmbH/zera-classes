
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
    if ( (scpiObject = m_pSCPICmdInterface->getSCPIObject(cmd)) != 0)
    {
        ScpiBaseDelegate* scpiDelegate = static_cast<ScpiBaseDelegate*>(scpiObject);
        return scpiDelegate->executeSCPI(client, cmd);
    }


    return false; // maybe that it is a common command
}

ScpiAmbiguityMap cSCPIInterface::checkAmbiguousShortNames()
{
    return m_pSCPICmdInterface->checkAmbiguousShortNames(ignoreAmbiguous);
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
                                                "CONF:EC01:0002:DUTC";
        for(const auto &ignoreEndShort : qAsConst(ignoreEndShortList)) {
            if(iter.key().endsWith(ignoreEndShort))
                ignore = true;
        }

        if(!ignore)
            outMap[iter.key()] = iter.value();
    }

    return outMap;
}


}
