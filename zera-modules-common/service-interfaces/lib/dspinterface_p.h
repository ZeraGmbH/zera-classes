#ifndef DSPINTERFACE_P_H
#define DSPINTERFACE_P_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "interface_p.h"
#include "dspinterface.h"

namespace Zera
{
namespace Server
{

enum dspcommands
{
    bootdsp,
    resetdsp,
    setbootpath,
    setsamplingsystem,
    varlist2dsp,
    cmdlist2dsp,
    intlist2dsp,
    setsignalrouting,
    setdsp61850sourceadr,
    setdsp61850destadr,
    setdsp61850prioritytagged,
    setdsp61850ethtypeappid,
    setdsp61850ethsynchronisation,
    resetmaximum,
    triggerinthksk,
    activateinterface,
    deactivateinterface,
    dataacquisition,
    dspmemoryread,
    dspmemorywrite,
    setgaincorrection,
    setphasecorrection,
    setoffsetcorrection,
    readdeviceversion,
    readserverversion,
    dspinterrupt
};


class cDSPInterface;

class cDSPInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT

public:
    cDSPInterfacePrivate(cDSPInterface* iface);
    virtual void setClient(Zera::Proxy::ProxyClient *client);
    virtual quint32 bootDsp();
    virtual quint32 resetDsp();
    virtual quint32 setDSPBootPath(QString path);
    virtual quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod
    virtual quint32 varList2Dsp(); // send the var-list to dsp server
    virtual quint32 cmdList2Dsp(); // send cyclic command list to the dsp server
    virtual quint32 intList2Dsp(); // send interrupt command list to the dsp server
    virtual int cmdListCount(); // returns the number of command in cyclist program list
    virtual int intListCount(); // returns the number of command in intlist program list
    virtual void clearCmdList(); // clears all cmd lists
    virtual void clearMemLists(); // clears all memory lists (memhandles)
    virtual quint32 setSignalRouting(tRouting* routingtab); // set signal routing
    virtual quint32 setDsp61850PriorityTagged(quint32 priotag); // set entries for eth frame decoder
    virtual quint32 setDsp61850EthTypeAppId(quint32 typAppid); // same
    virtual quint32 setDsp61850EthSynchronisation(quint32 syncdata); // setting strong sync. and dataset
    virtual quint32 resetMaximum(); // clear maximum memory
    virtual quint32 triggerIntHKSK(quint32); // trigger start hksk in intlist
    virtual void addCycListItem(QString cmd); // appends new command to cyclic list
    virtual void addIntListItem(QString cmd); // same for interrupt list
    virtual cDspMeasData* getMemHandle(QString name); // init a new memory group and return handle
    virtual void deleteMemHandle(cDspMeasData* memhandle);
    virtual quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)
    virtual quint32 deactivateInterface(); // unload ...
    virtual quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    virtual quint32 dspMemoryRead(cDspMeasData* memgroup, DSPDATA::dType type); // reads all vars of this memorygroup
    virtual void setVarData(cDspMeasData* memgroup, QString datalist, DSPDATA::dType type = DSPDATA::dFloat); //set the values of memgroup from qstring
    virtual quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type
    virtual float* data(cDspMeasData* memgroup, QString name); // returns a pointer to the vars read from dsp
    virtual void getData(cDspMeasData* memgroup, QVector<float>& vector); // copy the memgroups data into qvector
    virtual quint32 setGainCorrection(int chn, float val); // sets gaincorrection for 1 channel
    virtual quint32 setPhaseCorrection(int chn, float val); // sets phase correction for 1 channel
    virtual quint32 setOffsetCorrection(int chn, float val); // sets offset correction for 1 channel
    virtual quint32 readDeviceVersion(); // reads the dsp devices version
    virtual quint32 readServerVersion(); // what do you think ?

protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cDSPInterface)
    cDSPInterface *q_ptr;

    QStringList CycCmdList, IntCmdList;
    QList<cDspMeasData*> m_DspMemoryDataList; // eine liste mit zeigern auf dsp speicher
    QHash<quint32, cDspMeasData*> m_MsgNrMeasData;
    QHash<quint32, DSPDATA::dType> m_MsgNrMemType;
};

}
}


#endif // DSPINTERFACE_P_H
