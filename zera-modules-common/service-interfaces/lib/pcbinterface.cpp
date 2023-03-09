#include "pcbinterface.h"
#include "pcbinterface_p.h"

namespace Zera {

cPCBInterface::cPCBInterface() :
    d_ptr(new cPCBInterfacePrivate(this))
{
}

cPCBInterface::~cPCBInterface()
{
    delete d_ptr;
}


void cPCBInterface::setClient(Zera::ProxyClient *client)
{
    Q_D(cPCBInterface);
    return d->setClient(client);
}

void cPCBInterface::setClientSmart(Zera::ProxyClientPtr client)
{
    Q_D(cPCBInterface);
    return d->setClientSmart(client);
}

quint32 cPCBInterface::getChannelList()
{
    Q_D(cPCBInterface);
    return d->getChannelList();
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


quint32 cPCBInterface::getADCRejection(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->getADCRejection(chnName, rngName);
}


quint32 cPCBInterface::isAvail(QString chnName, QString rngName)
{
    Q_D(cPCBInterface);
    return d->isAvail(chnName, rngName);
}


quint32 cPCBInterface::getGainCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getGainCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjGainCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjGainCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getOffsetCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getOffsetCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjOffsetCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjOffsetCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getPhaseCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getPhaseCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjPhaseCorrection(QString chnName, QString rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjPhaseCorrection(chnName, rngName, at);
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


quint32 cPCBInterface::adjustComputation()
{
    Q_D(cPCBInterface);
    return d->adjustComputation();
}


quint32 cPCBInterface::adjustStorage()
{
    Q_D(cPCBInterface);
    return d->adjustStorage();
}


quint32 cPCBInterface::adjustStorageClamp()
{
    Q_D(cPCBInterface);
    return d->adjustStorageClamp();
}


quint32 cPCBInterface::setAdjustGainStatus(QString chnName, QString rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustGainStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setAdjustPhaseStatus(QString chnName, QString rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustPhaseStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setAdjustOffsetStatus(QString chnName, QString rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustOffsetStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setGainNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setGainNode(chnName, rngName, nr, corr, at);
}


quint32 cPCBInterface::setPhaseNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setPhaseNode(chnName, rngName, nr, corr, at);
}


quint32 cPCBInterface::setOffsetNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setOffsetNode(chnName, rngName, nr, corr, at);

}


quint32 cPCBInterface::getAliasSample(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSample(chnName);
}


quint32 cPCBInterface::registerNotifier(QString query, int notifyId)
{
    Q_D(cPCBInterface);
    return d->registerNotifier(query, notifyId);
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


quint32 cPCBInterface::writeSerialNr(QString serNr)
{
    Q_D(cPCBInterface);
    return d->writeSerialNr(serNr);
}


quint32 cPCBInterface::getAdjustmentChksum()
{
    Q_D(cPCBInterface);
    return d->getAdjustmentChksum();
}


quint32 cPCBInterface::getPCBAdjustmentData()
{
    Q_D(cPCBInterface);
    return d->getPCBAdjustmentData();
}


quint32 cPCBInterface::setPCBAdjustmentData(QString xmlpcb)
{
    Q_D(cPCBInterface);
    return d->setPCBAdjustmentData(xmlpcb);
}


quint32 cPCBInterface::getClampAdjustmentData()
{
    Q_D(cPCBInterface);
    return d->getClampAdjustmentData();
}


quint32 cPCBInterface::setClampAdjustmentData(QString xmlclamp)
{
    Q_D(cPCBInterface);
    return d->setClampAdjustmentData(xmlclamp);
}

quint32 cPCBInterface::getAccumulatorStatus()
{
    Q_D(cPCBInterface);
    return d->getAccumulatorStatus();
}

quint32 cPCBInterface::getAccumulatorSoc()
{
    Q_D(cPCBInterface);
    return d->getAccumulatorSoc();
}


quint32 cPCBInterface::transparentCommand(QString cmd)
{
    Q_D(cPCBInterface);
    return d->transparentCommand(cmd);
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


quint32 cPCBInterface::getPowTypeSource(QString chnName)
{
    Q_D(cPCBInterface);
    return d->getPowTypeSource(chnName);
}


quint32 cPCBInterface::setPowTypeSource(QString chnName, QString ptype)
{
    Q_D(cPCBInterface);
    return d->setPowTypeSource(chnName, ptype);
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


quint32 cPCBInterface::getAuthorizationStatus()
{
    Q_D(cPCBInterface);
    return d->getAuthorizationStatus();
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
