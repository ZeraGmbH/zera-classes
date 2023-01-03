#include "proxy.h"
#include "proxy_p.h"

namespace Zera { namespace Proxy {

cProxy* cProxy::getInstance()
{
    if(cProxyPrivate::singletonInstance == 0)
    {
        cProxyPrivate::singletonInstance=new cProxy;
    }
    return cProxyPrivate::singletonInstance;
}

cProxyClient* cProxy::getConnection(QString ipadress, quint16 port)
{
    Q_D(cProxy);
    return d->getConnection(ipadress, port);
}

ProxyClientPtr cProxy::getConnectionSmart(QString ipadress, quint16 port)
{
    Q_D(cProxy);
    return d->getConnectionSmart(ipadress, port);
}

void cProxy::startConnection(cProxyClient *client)
{
    Q_D(cProxy);
    d->startConnection((cProxyClientPrivate*)client);
}

void cProxy::startConnectionSmart(ProxyClientPtr client)
{
    Q_D(cProxy);
    d->startConnection((cProxyClientPrivate*)(client.get()));
}

bool cProxy::releaseConnection(cProxyClient *client)
{
    Q_D(cProxy);
    return d->releaseConnection((cProxyClientPrivate*) client);
}

cProxy::cProxy(QObject *parent):
    d_ptr(new Zera::Proxy::cProxyPrivate(this))
{
    setParent(parent);
}

cProxy::~cProxy()
{
    delete d_ptr;
}

}}
