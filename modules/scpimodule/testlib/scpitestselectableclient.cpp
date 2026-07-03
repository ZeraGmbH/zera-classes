#include "scpitestselectableclient.h"
#include "scpimodule.h"
#include <timemachineobject.h>

ScpiTestSelectableClient::ScpiTestSelectableClient(ClientType clientType, SCPIMODULE::cSCPIModule *scpiModule) :
    m_clientType(clientType)
{
    if (clientType == TEST) {
        m_testClient = std::make_unique<SCPIMODULE::ScpiTestClient>(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
        connect(m_testClient.get(), &SCPIMODULE::ScpiTestClient::sigScpiAnswer,
                this, &ScpiTestSelectableClient::onTestScpiAnswer);
    }
    else
        m_blockedClient = std::make_unique<ScpiModuleNetClientBlocked>();
}

QString ScpiTestSelectableClient::sendReceive(const QString &scpi)
{
    if (m_clientType == TEST) {
        m_testClient->sendScpiCmds(scpi);
        TimeMachineObject::feedEventLoop();
        return m_testClientResponse;
    }
    else
        return m_blockedClient->sendReceive(scpi.toLocal8Bit());

}

void ScpiTestSelectableClient::onTestScpiAnswer(const QString &scpiResponse)
{
    m_testClientResponse = scpiResponse;
}
