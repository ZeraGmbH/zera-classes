#ifndef PROXY_H
#define PROXY_H

#include "service-interfaces_export.h"
#include "proxyclient.h"
#include <functional>

namespace Zera {
class ProxyPrivate;

class SERVICE_INTERFACES_EXPORT Proxy : public QObject
{
    Q_OBJECT
public:
    static Proxy* getInstance();
    ProxyClient* getConnection(QString ipadress, quint16 port);
    ProxyClientPtr getConnectionSmart(QString ipadress, quint16 port);
    void startConnection(ProxyClient *client);
    void startConnectionSmart(ProxyClientPtr client);
    bool releaseConnection(ProxyClient* client);
protected:
    explicit Proxy(QObject* parent = 0);
    ~Proxy();
    ProxyPrivate *d_ptr;
private:
    Q_DISABLE_COPY(Proxy)
    Q_DECLARE_PRIVATE(Proxy)
};

}

#endif // PROXY_H
