#ifndef PROXICLIENT_P_H
#define PROXICLIENT_P_H

#include <QObject>
#include <netmessages.pb.h>
#include <proxiclient.h>


namespace Zera
{
namespace Proxi
{

class cProxiPrivate;

class cProxiClientPrivate: public cProxiClient
{
    Q_OBJECT

public:
    cProxiClientPrivate(cProxiPrivate* proxi);
    void transmitAnswer(ProtobufMessage::NetMessage *message);
    void transmitError(QAbstractSocket::SocketError errorCode);

protected:
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message);
    cProxiPrivate *m_pProxi;

};

}
}
#endif // PROXICLIENT_P_H
