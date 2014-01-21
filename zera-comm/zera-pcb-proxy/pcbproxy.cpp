#include "pcbproxy.h"
#include "pcbproxy_p.h"


namespace Zera
{
namespace PCBProxy
{

cPCBProxy *cPCBProxy::getInstance()
{
    if(cPCBProxyPrivate::singletonInstance == 0)
    {
        cPCBProxyPrivate::singletonInstance=new cPCBProxy;
    }
    return cPCBProxyPrivate::singletonInstance;
}


cInterface *cPCBProxy::getInterface(quint16 port)
{
    Q_D(cPCBProxy);
    return d->getInterface(port);
}


void cPCBProxy::setIPAdress(QString ipAddress)
{
    Q_D(cPCBProxy);
    d->setIPAdress(ipAddress);
}


cPCBProxy::cPCBProxy(QObject *parent):
    d_ptr(new Zera::PCBProxy::cPCBProxyPrivate(this))
{
    setParent(parent);
}


cPCBProxy::~cPCBProxy()
{
    delete d_ptr;
}

}
}
