#include "secinterface.h"
#include "secinterface_p.h"

namespace Zera {

cSECInterface::cSECInterface() :
    d_ptr(new cSECInterfacePrivate(this))
{
}

cSECInterface::~cSECInterface()
{
    delete d_ptr;
}


void cSECInterface::setClient(Zera::ProxyClient *client)
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


quint32 cSECInterface::setMux(QString chnname, QString inpname)
{
    Q_D(cSECInterface);
    return d->setMux(chnname, inpname);
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


quint32 cSECInterface::intAck(QString chnname, quint8 interrupt)
{
    Q_D(cSECInterface);
    return d->intAck(chnname, interrupt);
}


quint32 cSECInterface::registerNotifier(QString query)
{
    Q_D(cSECInterface);
    return d->registerNotifier(query);
}


quint32 cSECInterface::unregisterNotifiers()
{
    Q_D(cSECInterface);
    return d->unregisterNotifiers();
}

}
