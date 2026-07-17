#include "scpidelegatemeasure.h"
#include "scpiclient.h"
#include <zscpi_response_definitions.h>

namespace SCPIMODULE {

ScpiDelegateMeasure::ScpiDelegateMeasure(const Params &params) :
    ScpiDelegateTemplate(params.cmdParent, params.cmd, params.scpiQueryCmdFlags),
    m_params(params)
{
}

void ScpiDelegateMeasure::addComponentId(const VeinComponentId &componentId)
{
    m_veinComponents.append(componentId);
}

void ScpiDelegateMeasure::executeSCPI(cSCPIClient *client,
                                      const QString &scpi,
                                      const ScpiTransactionId &scpiTransactionId)
{
    cSCPICommand cmd = scpi;
    if ( (cmd.isQuery() && ((m_params.scpiQueryCmdFlags & SCPI::isQuery) > 0)) ||
        (cmd.isCommand(0) && ((m_params.scpiQueryCmdFlags & SCPI::isCmd) > 0)) ) {
        // allowed query or command
        getExecutor(client)->executeScpi(scpiTransactionId);
    }
    else
        client->handleCmdFinishStatusOnly(ZSCPI::nak, scpiTransactionId);
}

ScpiClientExecutorMeasurePtr ScpiDelegateMeasure::getExecutor(cSCPIClient *client)
{
    QUuid clientId = client->getClientId();
    auto iter = m_clientExecutors.constFind(clientId);
    if (iter != m_clientExecutors.constEnd())
        return iter.value();

    ScpiClientExecutorMeasurePtr executor = std::make_shared<ScpiClientExecutorMeasure>(client,
                                                                                        m_params.modelType,
                                                                                        m_veinComponents);

    m_clientExecutors[clientId] = executor;
    return executor;
}

}
