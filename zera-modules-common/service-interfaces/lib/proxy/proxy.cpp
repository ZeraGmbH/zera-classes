#include "proxy.h"
#include "proxy_p.h"

namespace Zera {

Proxy* Proxy::getInstance()
{
    if(ProxyPrivate::singletonInstance == 0)
    {
        ProxyPrivate::singletonInstance=new Proxy;
    }
    return ProxyPrivate::singletonInstance;
}

ProxyClient* Proxy::getConnection(QString ipadress, quint16 port)
{
    Q_D(Proxy);
    return d->getConnection(ipadress, port);
}

ProxyClientPtr Proxy::getConnectionSmart(QString ipadress, quint16 port)
{
    Q_D(Proxy);
    return d->getConnectionSmart(ipadress, port);
}

void Proxy::startConnection(ProxyClient *client)
{
    Q_D(Proxy);
    d->startConnection((ProxyClientPrivate*)client);
}

void Proxy::startConnectionSmart(ProxyClientPtr client)
{
    Q_D(Proxy);
    d->startConnection((ProxyClientPrivate*)(client.get()));
}

bool Proxy::releaseConnection(ProxyClient *client)
{
    Q_D(Proxy);
    return d->releaseConnection((ProxyClientPrivate*) client);
}

Proxy::Proxy(QObject *parent):
    d_ptr(new Zera::ProxyPrivate(this))
{
    setParent(parent);
}

Proxy::~Proxy()
{
    delete d_ptr;
}

}
