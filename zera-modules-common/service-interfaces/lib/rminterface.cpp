#include "rminterface.h"
#include "rminterface_p.h"


namespace Zera
{
namespace Server
{

cRMInterface::cRMInterface() :
    d_ptr(new cRMInterfacePrivate(this))
{
}

cRMInterface::~cRMInterface()
{
    delete d_ptr;
}


void cRMInterface::setClientSmart(Proxy::ProxyClientPtr client)
{
    Q_D(cRMInterface);
    return d->setClientSmart(client);
}


quint32 cRMInterface::rmIdent(QString name)
{
    Q_D(cRMInterface);
    return d->rmIdent(name);
}


quint32 cRMInterface::addResource(QString type, QString name, int n, QString description, quint16 port)
{
    Q_D(cRMInterface);
    return d->addResource(type, name, n, description, port);
}


quint32 cRMInterface::removeResource(QString type, QString name)
{
    Q_D(cRMInterface);
    return d->removeResource(type, name);
}


quint32 cRMInterface::getResourceTypes()
{
    Q_D(cRMInterface);
    return d->getResourceTypes();
}


quint32 cRMInterface::getResources(QString type)
{
    Q_D(cRMInterface);
    return d->getResources(type);
}


quint32 cRMInterface::getResourceInfo(QString type, QString name)
{
    Q_D(cRMInterface);
    return d->getResourceInfo(type, name);
}


quint32 cRMInterface::setResource(QString type, QString name, int n)
{
    Q_D(cRMInterface);
    return d->setResource(type, name, n);
}


quint32 cRMInterface::freeResource(QString type, QString name)
{
    Q_D(cRMInterface);
    return d->freeResource(type, name);
}

}
}
