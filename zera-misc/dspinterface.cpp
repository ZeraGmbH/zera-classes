
#include "dspinterface.h"
#include "dspinterface_p.h"

namespace Zera
{
namespace Server
{


cDSPInterface::cDSPInterface()
    :d_ptr(new cDSPInterfacePrivate(this))
{
}


void cDSPInterface::setClient(Proxy::cProxyClient *client)
{
    Q_D(cDSPInterface);
    return d->setClient(client);
}


quint32 cDSPInterface::bootDsp()
{
    Q_D(cDSPInterface);
    return d->bootDsp();
}


quint32 cDSPInterface::resetDsp()
{
    Q_D(cDSPInterface);
    return d->resetDsp();
}


quint32 cDSPInterface::setDSPBootPath(QString path)
{
    Q_D(cDSPInterface);
    return d->setDSPBootPath(path);
}


quint32 cDSPInterface::setSamplingSystem(int chncount, int samp_per, int samp_mper)
{
    Q_D(cDSPInterface);
    return d->setSamplingSystem(chncount, samp_per, samp_mper);
}


quint32 cDSPInterface::varList2Dsp()
{
    Q_D(cDSPInterface);
    return d->varList2Dsp();
}


quint32 cDSPInterface::cmdLists2Dsp()
{
    Q_D(cDSPInterface);
    return d->cmdLists2Dsp();
}


void cDSPInterface::clearCmdList()
{
    Q_D(cDSPInterface);
    d->clearCmdList();
}


void cDSPInterface::clearVarLists()
{
    Q_D(cDSPInterface);
    d->clearVarLists();
}


void cDSPInterface::ClearMemLists()
{
    Q_D(cDSPInterface);
    d->ClearMemLists();
}


quint32 cDSPInterface::setSignalRouting(tRouting *routingtab)
{
    Q_D(cDSPInterface);
    return d->setSignalRouting(routingtab);
}


quint32 cDSPInterface::setDsp61850SourceAdr(cETHAdress &ethadr)
{
    Q_D(cDSPInterface);
    return d->setDsp61850SourceAdr(ethadr);
}


quint32 cDSPInterface::setDsp61850DestAdr(cETHAdress &ethadr)
{
    Q_D(cDSPInterface);
    return d->setDsp61850DestAdr(ethadr);
}


quint32 cDSPInterface::setDsp61850PriorityTagged(quint32 priotag)
{
    Q_D(cDSPInterface);
    return d->setDsp61850PriorityTagged(priotag);
}


quint32 cDSPInterface::setDsp61850EthTypeAppId(quint32 typAppid)
{
    Q_D(cDSPInterface);
    return d->setDsp61850EthTypeAppId(typAppid);
}


quint32 cDSPInterface::setDsp61850EthSynchronisation(quint32 syncdata)
{
    Q_D(cDSPInterface);
    return d->setDsp61850EthSynchronisation(syncdata);
}


quint32 cDSPInterface::resetMaximum()
{
    Q_D(cDSPInterface);
    return d->resetMaximum();
}


quint32 cDSPInterface::triggerIntHKSK(quint32 hksk)
{
    Q_D(cDSPInterface);
    return d->triggerIntHKSK(hksk);
}


void cDSPInterface::addCycListItem(QString cmd)
{
    Q_D(cDSPInterface);
    d->addCycListItem(cmd);
}


void cDSPInterface::addIntListItem(QString cmd)
{
    Q_D(cDSPInterface);
    d->addIntListItem(cmd);
}


cDspMeasData* cDSPInterface::getMemHandle(QString name, DSPDATA::DspSegType segtype)
{
    Q_D(cDSPInterface);
    return d->getMemHandle(name, segtype);
}


void cDSPInterface::deleteDSPMemHandle(cDspMeasData *memhandle)
{
    Q_D(cDSPInterface);
    d->deleteDSPMemHandle(memhandle);
}


void cDSPInterface::addVarItem(cDspMeasData *memgroup, cDspVar *var)
{
    Q_D(cDSPInterface);
    d->addVarItem(memgroup, var);
}


quint32 cDSPInterface::activateInterface()
{
    Q_D(cDSPInterface);
    return d->activateInterface();
}


quint32 cDSPInterface::deactivateInterface()
{
    Q_D(cDSPInterface);
    return d->deactivateInterface();
}


quint32 cDSPInterface::dataAcquisition(cDspMeasData *memgroup)
{
    Q_D(cDSPInterface);
    return d->dataAcquisition(memgroup);
}


quint32 cDSPInterface::dspMemoryRead(cDspMeasData *memgroup)
{
    Q_D(cDSPInterface);
    return d->dspMemoryRead(memgroup);
}


float* cDSPInterface::setVarData(cDspMeasData *memgroup, QString s)
{
    Q_D(cDSPInterface);
    return d->setVarData(memgroup, s);
}


quint32 cDSPInterface::dspMemoryWrite(cDspMeasData *memgroup, DSPDATA::dType type)
{
    Q_D(cDSPInterface);
    return d->dspMemoryWrite(memgroup, type);
}


float* cDSPInterface::data(cDspMeasData *dspdata)
{
    Q_D(cDSPInterface);
    return d->data(dspdata);
}


quint32 cDSPInterface::setGainCorrection(int chn, float val)
{
    Q_D(cDSPInterface);
    return d->setGainCorrection(chn, val);
}


quint32 cDSPInterface::setPhaseCorrection(int chn, float val)
{
    Q_D(cDSPInterface);
    return d->setPhaseCorrection(chn, val);
}


quint32 cDSPInterface::setOffsetCorrection(int chn, float val)
{
    Q_D(cDSPInterface);
    return d->setOffsetCorrection(chn, val);
}


quint32 cDSPInterface::readDeviceVersion()
{
    Q_D(cDSPInterface);
    return d->readDeviceVersion();
}


quint32 cDSPInterface::readServerVersion()
{
    Q_D(cDSPInterface);
    return d->readServerVersion();
}


}
}
