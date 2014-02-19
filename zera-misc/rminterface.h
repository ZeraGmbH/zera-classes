#ifndef RMINTERFACE_H
#define RMINTERFACE_H

#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

#include "zera_misc_global.h"


namespace Zera
{
namespace Proxy
{
    class cProxyClient;
}
}

namespace Zera
{
namespace Server
{

class cRMInterfacePrivate;


class ZERA_MISCSHARED_EXPORT cRMInterface: public QObject
{
    Q_OBJECT

public:
    cRMInterface();
    virtual void setClient(Zera::Proxy::cProxyClient *client);
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
    void serverAnswer(quint32 msgnr, QVariant answer);

private:
    Q_DECLARE_PRIVATE(cRMInterface)
    cRMInterfacePrivate *d_ptr;
};

}
}


#endif // RMINTERFACE_H
