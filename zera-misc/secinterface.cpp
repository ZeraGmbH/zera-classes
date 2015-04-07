#include "secinterface.h"
#include "secinterface_p.h"


namespace Zera
{
namespace Server
{

cSECInterface::cSECInterface()
    :d_ptr(new cSECInterfacePrivate(this))
{
}


void cSECInterface::setClient(Proxy::cProxyClient *client)
{
    Q_D(cSECInterface);
    return d->setClient(client);
}


quint32 cSECInterface::setECalcUnit(int n)
{
    Q_D(cSECInterface);
    return d->setECalcUnit(n);
}


quint32 cSECInterface::freeECalcUnits()
{
    Q_D(cSECInterface);
    return d->freeECalcUnits();
}


quint32 cSECInterface::writeRegister(QString chnname, quint8 reg, quint32 value)
{
    Q_D(cSECInterface);
    return d->writeRegister(chnname, reg, value);
}


quint32 cSECInterface::readRegister(QString chnname, quint8 reg)
{
    Q_D(cSECInterface);
    return d->readRegister(chnname, reg);
}


quint32 cSECInterface::setSync(QString chnname, QString syncchn)
{
    Q_D(cSECInterface);
    return d->setSync(chnname, syncchn);
}


quint32 cSECInterface::setMux(QString chnname, quint8 mux)
{
    Q_D(cSECInterface);
    return d->setMux(chnname, mux);
}


quint32 cSECInterface::setCmdid(QString chnname, quint8 cmdid)
{
    Q_D(cSECInterface);
    return d->setCmdid(chnname, cmdid);
}


quint32 cSECInterface::start(QString chnname)
{
    Q_D(cSECInterface);
    return d->start(chnname);
}


quint32 cSECInterface::stop(QString chnname)
{
    Q_D(cSECInterface);
    return d->stop(chnname);
}


quint32 cSECInterface::registerNotifier(QString query, QString notifier)
{
    Q_D(cSECInterface);
    return d->registerNotifier(query, notifier);
}


quint32 cSECInterface::unregisterNotifiers()
{
    Q_D(cSECInterface);
    return d->unregisterNotifiers();
}


}
}

