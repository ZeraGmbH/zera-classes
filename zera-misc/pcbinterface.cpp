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


quint32 cPCBInterface::getUrvalue(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getUrvalue(chnName);
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


quint32 cPCBInterface::setMMode(QString mmode)
{
    Q_D(cPCBInterface);
    return d->setMMode(mmode);
}


quint32 cPCBInterface::getAliasSample(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSample(chnName);
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


quint32 cPCBInterface::readServerVersion()
{
    Q_D(cPCBInterface);
    return d->readServerVersion();
}


quint32 cPCBInterface::readPCBVersion()
{
    Q_D(cPCBInterface);
    return d->readPCBVersion();
}


quint32 cPCBInterface::readFPGAVersion()
{
    Q_D(cPCBInterface);
    return d->readFPGAVersion();
}


quint32 cPCBInterface::readCTRLVersion()
{
    Q_D(cPCBInterface);
    return d->readCTRLVersion();
}


quint32 cPCBInterface::readSerialNr()
{
    Q_D(cPCBInterface);
    return d->readSerialNr();
}


quint32 cPCBInterface::getAdjustmentChksum()
{
    Q_D(cPCBInterface);
    return d->getAdjustmentChksum();
}


quint32 cPCBInterface::getSampleRate()
{
    Q_D(cPCBInterface);
    return d->getSampleRate();
}


quint32 cPCBInterface::getRangeListSample(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getRangeListSample(chnName);
}


quint32 cPCBInterface::setRangeSample(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->setRangeSample(chnName, rngName);
}


quint32 cPCBInterface::setPLLChannel(QString samplechnName, QString pllchnName)
{
    Q_D(cPCBInterface);
    return d->setPLLChannel(samplechnName, pllchnName);
}


quint32 cPCBInterface::getAliasSource(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSource(chnName);
}


quint32 cPCBInterface::getDSPChannelSource(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getDSPChannelSource(chnName);
}


quint32 cPCBInterface::getFormFactorSource(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getFormFactorSource(chnName);
}


quint32 cPCBInterface::getConstantSource(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getConstantSource(chnName);
}


quint32 cPCBInterface::setConstantSource(QString chnName, double constant)
{
    Q_D(cPCBInterface);
    return d->setConstantSource(chnName, constant);
}


quint32 cPCBInterface::getAliasSchead(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSchead(chnName);
}


quint32 cPCBInterface::getAliasFrqinput(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasFrqinput(chnName);
}


quint32 cPCBInterface::getAdjustmentStatus()
{
    Q_D(cPCBInterface);
    return d->getAdjustmentStatus();
}

quint32 cPCBInterface::getPCBErrorStatus()
{

    Q_D(cPCBInterface);
    return d->getPCBErrorStatus();
}


quint32 cPCBInterface::resourceAliasQuery(QString resourceType, QString resourceName)
{
    Q_D(cPCBInterface);
    return d->resourceAliasQuery(resourceType, resourceName);
}
















}
}
