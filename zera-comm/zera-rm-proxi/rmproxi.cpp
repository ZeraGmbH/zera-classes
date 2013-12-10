#include "rmproxi.h"
#include "rmproxi_p.h"

namespace Zera
{
namespace RMProxi
{

cRMProxi *cRMProxi::getInstance()
{
    if(cRMProxiPrivate::singletonInstance==0)
    {
        cRMProxiPrivate::singletonInstance=new cRMProxi;
    }
    return cRMProxiPrivate::singletonInstance;
}


cInterface *cRMProxi::getInterface()
{
    Q_D(cRMProxi);
    d->getInterface();
}


void cRMProxi::startNetwork(QString ipAddress, quint16 port)
{
    Q_D(cRMProxi);
    d->startNetwork(ipAddress, port);
}


cRMProxi::cRMProxi(QObject *parent):
    d_ptr(new Zera::RMProxi::cRMProxiPrivate(this))
{
    setParent(parent);
}


cRMProxi::~cRMProxi()
{
    delete d_ptr;
}

}
}

