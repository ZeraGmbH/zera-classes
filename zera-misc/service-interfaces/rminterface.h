#ifndef RMINTERFACE_H
#define RMINTERFACE_H

#include "zeramisc_export.h"
#include <proxyclient.h>
#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

namespace Zera
{
namespace Server
{
class cRMInterfacePrivate;

class ZERAMISC_EXPORT cRMInterface: public QObject
{
    Q_OBJECT
public:
    cRMInterface();
    virtual ~cRMInterface();
    void setClientSmart(Zera::Proxy::ProxyClientPtr client);
    virtual quint32 rmIdent(QString name);
    virtual quint32 addResource(QString type, QString name, int n, QString description, quint16 port);
    virtual quint32 removeResource(QString type, QString name);
    virtual quint32 getResourceTypes();
    virtual quint32 getResources(QString type);
    virtual quint32 getResourceInfo(QString type, QString name);
    virtual quint32 setResource(QString type, QString name, int n);
    virtual quint32 freeResource(QString type, QString name);

signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    Q_DECLARE_PRIVATE(cRMInterface)
    cRMInterfacePrivate *d_ptr;
};

}
}


#endif // RMINTERFACE_H
