#include "iotransferdemoresponder.h"

const QByteArray IoTransferDemoResponder::errorResponseData = QByteArrayLiteral("_ERROR_RESPONSE_");

IoTransferDemoResponder::IoTransferDemoResponder(QByteArray expectedDataLead, QByteArray expectedDataTrail) :
    m_expectedDataLead(expectedDataLead),
    m_expectedDataTrail(expectedDataTrail)
{
}

void IoTransferDemoResponder::activateErrorResponse()
{
    m_responseOverride = errorResponseData;
}

void IoTransferDemoResponder::overrideDefaultResponse(QByteArray override)
{
    m_responseOverride = override;
}

QByteArray IoTransferDemoResponder::getDemoResponse() const
{
    return m_responseOverride.isEmpty() ? m_expectedDataLead+m_expectedDataTrail : m_responseOverride;
}

QByteArray IoTransferDemoResponder::getDefaultErrorResponse()
{
    return errorResponseData;
}
