#ifndef IOTRANSFERDEMORESPONDER_H
#define IOTRANSFERDEMORESPONDER_H

#include "QByteArray"
#include "QSharedPointer"

class IoTransferDemoResponder
{
public:
    typedef QSharedPointer<IoTransferDemoResponder> Ptr;
    IoTransferDemoResponder(QByteArray expectedDataLead, QByteArray expectedDataTrail);
    void activateErrorResponse();
    void overrideDefaultResponse(QByteArray override);
    QByteArray getDemoResponse() const;
    static QByteArray getDefaultErrorResponse();

private:
    QByteArray m_expectedDataLead;
    QByteArray m_expectedDataTrail;
    QByteArray m_responseOverride;
    static const QByteArray errorResponseData;
};

#endif // IOTRANSFERDEMORESPONDER_H
