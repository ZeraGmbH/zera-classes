#include "proxi.h"
#include "proxi_p.h"

namespace Zera
{
namespace Proxi
{

cProxi* cProxi::getInstance()
{
    if(cProxiPrivate::singletonInstance == 0)
    {
        cProxiPrivate::singletonInstance=new cProxi;
    }
    return cProxiPrivate::singletonInstance;
}


cProxiClient* cProxi::getConnection(QString ipadress, quint16 port)
{
    Q_D(cProxi);
    return d->getConnection(ipadress, port);
}


cProxiClient* cProxi::getConnection(quint16 port)
{
    Q_D(cProxi);
    return d->getConnection(port);
}


void cProxi::setIPAdress(QString ipAddress)
{
    Q_D(cProxi);
    d->setIPAdress(ipAddress);
}


cProxi::cProxi(QObject *parent):
    d_ptr(new Zera::Proxi::cProxiPrivate(this))
{
    setParent(parent);
}


cProxi::~cProxi()
{
    delete d_ptr;
}

}
}
