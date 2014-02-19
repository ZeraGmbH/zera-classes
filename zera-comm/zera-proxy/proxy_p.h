#ifndef PROXI_P_H
#define PROXI_P_H

#include <QObject>
#include <QHash>
#include <QString>
#include <zeraclientnetbase.h>
#include <netmessages.pb.h>

#include "proxi.h"


namespace Zera
{
namespace Proxi
{

class cProxi;
class cProxiClient;
class cProxiClientPrivate;
class cProxiConnection;

class cProxiPrivate: public QObject
{
    Q_OBJECT

public:
    quint32 transmitCommand(cProxiClientPrivate *client, ProtobufMessage::NetMessage *message);

protected:
    cProxiPrivate(cProxi *parent);
    ~cProxiPrivate(){}
    cProxiClient* getConnection(QString ipadress, quint16 port);
    cProxiClient* getConnection(quint16 port);
    void setIPAdress(QString ipAddress);

    /**
       * @brief singletonInstance
       */
    static cProxi* singletonInstance;

    /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
    cProxi *q_ptr;

protected slots:
    void receiveMessage(QByteArray message);
    void receiveTcpError(QAbstractSocket::SocketError errorCode);

private:
    Q_DISABLE_COPY(cProxiPrivate)
    Q_DECLARE_PUBLIC(cProxi)

    Zera::NetClient::cClientNetBase* searchConnection(QString ip, quint16 port); // we search for a netclient that matches ip, port
    QHash<cProxiClientPrivate*, cProxiConnection*> m_ConnectionHash; // holds network connection for each client
    QHash<QByteArray, cProxiClientPrivate*> m_ClientHash; // information for faster redirecting
    QString m_sIPAdress; // ip adress for all zera servers, default localhost
    quint32 m_nMessageNumber; // message number, .. we never use 0    

};

}
}



#endif // PROXI_P_H
