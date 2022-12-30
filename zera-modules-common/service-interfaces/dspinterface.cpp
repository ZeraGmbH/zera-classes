
#include "dspinterface.h"
#include "dspinterface_p.h"

namespace Zera
{
namespace Server
{


cDSPInterface::cDSPInterface() :
    d_ptr(new cDSPInterfacePrivate(this))
{
}

cDSPInterface::~cDSPInterface()
{
    delete d_ptr;
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


quint32 cDSPInterface::cmdList2Dsp()
{
    Q_D(cDSPInterface);
    return d->cmdList2Dsp();
}


quint32 cDSPInterface::intList2Dsp()
{
    Q_D(cDSPInterface);
    return d->intList2Dsp();
}


int cDSPInterface::cmdListCount()
{
    Q_D(cDSPInterface);
    return d->cmdListCount();
}


int cDSPInterface::intListCount()
{
    Q_D(cDSPInterface);
    return d->intListCount();
}


void cDSPInterface::clearCmdList()
{
    Q_D(cDSPInterface);
    d->clearCmdList();
}


void cDSPInterface::clearMemLists()
{
    Q_D(cDSPInterface);
    d->clearMemLists();
}


quint32 cDSPInterface::setSignalRouting(tRouting *routingtab)
{
    Q_D(cDSPInterface);
    return d->setSignalRouting(routingtab);
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


cDspMeasData* cDSPInterface::getMemHandle(QString name)
{
    Q_D(cDSPInterface);
    return d->getMemHandle(name);
}


void cDSPInterface::deleteMemHandle(cDspMeasData *memhandle)
{
    Q_D(cDSPInterface);
    d->deleteMemHandle(memhandle);
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


quint32 cDSPInterface::dspMemoryRead(cDspMeasData *memgroup, DSPDATA::dType type)
{
    Q_D(cDSPInterface);
    return d->dspMemoryRead(memgroup, type);
}


void cDSPInterface::setVarData(cDspMeasData *memgroup, QString datalist, DSPDATA::dType type)
{
    Q_D(cDSPInterface);
    d->setVarData(memgroup, datalist, type);
}


quint32 cDSPInterface::dspMemoryWrite(cDspMeasData *memgroup)
{
    Q_D(cDSPInterface);
    return d->dspMemoryWrite(memgroup);
}


float* cDSPInterface::data(cDspMeasData *memgroup, QString name)
{
    Q_D(cDSPInterface);
    return d->data(memgroup, name);
}


void cDSPInterface::getData(cDspMeasData *memgroup, QVector<float> &vector)
{
    Q_D(cDSPInterface);
    d->getData(memgroup, vector);
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
