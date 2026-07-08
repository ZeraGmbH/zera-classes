#include "scpitestselectableclient.h"
#include "scpimodule.h"
#include <timemachineobject.h>

ScpiTestSelectableClient::ScpiTestSelectableClient(ClientType clientType, SCPIMODULE::cSCPIModule *scpiModule) :
    m_clientType(clientType)
{
    if (clientType == TEST) {
        m_testClient = std::make_unique<SCPIMODULE::ScpiTestClient>(scpiModule, *scpiModule->getConfigData(), scpiModule->getSCPIServer()->getScpiInterface());
        connect(m_testClient.get(), &SCPIMODULE::ScpiTestClient::sigScpiResponseSorted,
                this, &ScpiTestSelectableClient::onTestScpiAnswer);
    }
    else
        m_blockedClient = std::make_unique<ScpiModuleNetClientBlocked>();
}

QString ScpiTestSelectableClient::sendReceive(const QString &scpi, bool removeLineFeedOnReceive)
{
    if (m_clientType == TEST) {
        m_testClient->sendScpiCmds(scpi);
        TimeMachineObject::feedEventLoop();
        QString response = m_testClientResponse;
        if (removeLineFeedOnReceive)
            response.remove("\n");
        return response;
    }
    else
        return m_blockedClient->sendReceive(scpi.toLocal8Bit(), removeLineFeedOnReceive);
}

bool ScpiTestSelectableClient::commandsPending() const
{
    if (m_clientType == TEST) {
        bool commandsPerfomedAndCompleted = m_testClient->getAtLeastOneResponse() &&
                                            m_testClient->getUnhandledResponses() == 0;
        return !commandsPerfomedAndCompleted;
    }
    else
        // No way to deduce -> assume all done
        return false;
}

void ScpiTestSelectableClient::onTestScpiAnswer(const QString &scpiResponse)
{
    m_testClientResponse = scpiResponse;
}
