#include "rmproxy.h"
#include "rmproxy_p.h"

namespace Zera
{
namespace RMProxy
{

cRMProxy *cRMProxy::getInstance()
{
    if(cRMProxyPrivate::singletonInstance==0)
    {
        cRMProxyPrivate::singletonInstance=new cRMProxy;
    }
    return cRMProxyPrivate::singletonInstance;
}


cInterface *cRMProxy::getInterface()
{
    Q_D(cRMProxy);
    return d->getInterface();
}


void cRMProxy::startNetwork(QString ipAddress, quint16 port)
{
    Q_D(cRMProxy);
    d->startNetwork(ipAddress, port);
}


cRMProxy::cRMProxy(QObject *parent):
    d_ptr(new Zera::RMProxy::cRMProxyPrivate(this))
{
    setParent(parent);
}


cRMProxy::~cRMProxy()
{
    delete d_ptr;
}

}
}

