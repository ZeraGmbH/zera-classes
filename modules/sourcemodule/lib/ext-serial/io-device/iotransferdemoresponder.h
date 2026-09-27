#ifndef IOTRANSFERDEMORESPONDER_H
#define IOTRANSFERDEMORESPONDER_H

#include "QByteArray"
#include "QSharedPointer"

class IoTransferDemoResponder
{
public:
    typedef QSharedPointer<IoTransferDemoResponder> Ptr;
    IoTransferDemoResponder(const QByteArray &expectedDataLead, const QByteArray &expectedDataTrail);
    void activateErrorResponse();
    void overrideDefaultResponse(const QByteArray &override);
    QByteArray getDemoResponse() const;
    static QByteArray getDefaultErrorResponse();

private:
    QByteArray m_expectedDataLead;
    QByteArray m_expectedDataTrail;
    QByteArray m_responseOverride;
    static const QByteArray errorResponseData;
};

#endif // IOTRANSFERDEMORESPONDER_H
