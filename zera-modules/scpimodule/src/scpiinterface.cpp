
#include <scpi.h>
#include <scpiobject.h>
#include <scpicommand.h>
#include <QString>

#include "scpiinterface.h"
#include "scpiclient.h"
#include "scpidelegate.h"


namespace SCPIMODULE
{

cSCPIInterface::cSCPIInterface(QString name)
    :m_sName(name)
{
    m_pSCPICmdInterface = new cSCPI(m_sName);
}


cSCPIInterface::~cSCPIInterface()
{
    delete m_pSCPICmdInterface;
}


cSCPI* cSCPIInterface::getSCPICmdInterface()
{
    return m_pSCPICmdInterface;
}


void cSCPIInterface::addSCPICommand(cSCPIDelegate *delegate)
{
    delegate->setCommand(m_pSCPICmdInterface);
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


}
