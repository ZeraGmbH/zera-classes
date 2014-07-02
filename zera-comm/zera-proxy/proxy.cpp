#include "proxy.h"
#include "proxy_p.h"

namespace Zera
{
namespace Proxy
{

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


cProxyClient* cProxy::getConnection(quint16 port)
{
    Q_D(cProxy);
    return d->getConnection(port);
}


bool cProxy::releaseConnection(cProxyClient *client)
{
    Q_D(cProxy);
    return d->releaseConnection((cProxyClientPrivate*) client);
}


void cProxy::setIPAdress(QString ipAddress)
{
    Q_D(cProxy);
    d->setIPAdress(ipAddress);
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

}
}
