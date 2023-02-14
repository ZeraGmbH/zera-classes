#ifndef PROXY_H
#define PROXY_H

#include "service-interfaces_export.h"
#include "proxyclient.h"
#include <QObject>
#include <functional>

namespace Zera { namespace Proxy
{
class cProxyPrivate;

class SERVICE_INTERFACES_EXPORT cProxy: public QObject
{
    Q_OBJECT
public:
    static cProxy* getInstance();
    cProxyClient* getConnection(QString ipadress, quint16 port);
    ProxyClientPtr getConnectionSmart(QString ipadress, quint16 port);
    void startConnection(cProxyClient *client);
    void startConnectionSmart(ProxyClientPtr client);
    bool releaseConnection(cProxyClient* client);
protected:
    explicit cProxy(QObject* parent = 0);
    ~cProxy();
    cProxyPrivate *d_ptr;
private:
    Q_DISABLE_COPY(cProxy)
    Q_DECLARE_PRIVATE(cProxy)
};

}}

#endif // PROXY_H
