#include "pcbinterface.h"
#include "pcbinterface_p.h"

namespace Zera
{
namespace Server
{


cPCBInterface::cPCBInterface()
    :d_ptr(new cPCBInterfacePrivate(this))
{
}


void cPCBInterface::setClient(Proxy::cProxyClient *client)
{
    Q_D(cPCBInterface);
    return d->setClient(client);
}


quint32 cPCBInterface::getDSPChannel(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getDSPChannel(chnName);
}


quint32 cPCBInterface::getStatus(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getStatus(chnName);
}


quint32 cPCBInterface::resetStatus(QString chnName)
{
    Q_D(cPCBInterface);
    return d->resetStatus(chnName);
}


quint32 cPCBInterface::getAlias(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAlias(chnName);
}


quint32 cPCBInterface::getType(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getType(chnName);
}


quint32 cPCBInterface::getUnit(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getUnit(chnName);
}


quint32 cPCBInterface::getRange(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getRange(chnName);
}


quint32 cPCBInterface::getRangeList(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getRangeList(chnName);
}


quint32 cPCBInterface::getAlias(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getAlias(chnName, rngName);
}


quint32 cPCBInterface::getType(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getType(chnName, rngName);
}


quint32 cPCBInterface::getUrvalue(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getUrvalue(chnName, rngName);
}


quint32 cPCBInterface::getRejection(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getRejection(chnName, rngName);
}


quint32 cPCBInterface::getOVRejection(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getOVRejection(chnName, rngName);
}


quint32 cPCBInterface::isAvail(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->isAvail(chnName, rngName);
}


quint32 cPCBInterface::getGainCorrection(QString chnName, QString rngName, double ampl)
{
    Q_D(cPCBInterface);
    return d->getGainCorrection(chnName, rngName, ampl);
}


quint32 cPCBInterface::getOffsetCorrection(QString chnName, QString rngName, double ampl)
{
    Q_D(cPCBInterface);
    return d->getOffsetCorrection(chnName, rngName, ampl);
}


quint32 cPCBInterface::getPhaseCorrection(QString chnName, QString rngName, double ampl)
{
    Q_D(cPCBInterface);
    return d->getPhaseCorrection(chnName, rngName, ampl);
}


quint32 cPCBInterface::setRange(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->setRange(chnName, rngName);
}


quint32 cPCBInterface::registerNotifier(QString query, QString notifier)
{
    Q_D(cPCBInterface);
    return d->registerNotifier(query, notifier);
}


quint32 cPCBInterface::unregisterNotifiers()
{
    Q_D(cPCBInterface);
    return d->unregisterNotifiers();
}


quint32 cPCBInterface::getSampleRate()
{
    Q_D(cPCBInterface);
    return d->getSampleRate();
}

















}
}
