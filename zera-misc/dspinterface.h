#ifndef DSPINTERFACE_H
#define DSPINTERFACE_H

#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

#include "zera_misc_global.h"
#include "ethadress.h"
#include "dspmeasdata.h"

class cDspVar;


namespace Zera
{
namespace Proxi
{
    class cProxiClient;
}
}


namespace Zera
{
namespace Server
{

typedef quint32 tRouting[16];

class cDSPInterfacePrivate;


class ZERA_MISCSHARED_EXPORT cDSPInterface: public QObject
{
    Q_OBJECT

public:
    cDSPInterface();
    virtual void setClient(Zera::Proxi::cProxiClient *client);
    virtual quint32 bootDsp();
    virtual quint32 resetDsp();
    virtual quint32 setDSPBootPath(QString path);
    virtual quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod
    virtual quint32 varList2Dsp(); // send the var-list to dsp server
    virtual quint32 cmdLists2Dsp(); // send cyclic- and intlist to the dsp server
    virtual void clearCmdList(); // clears all cmd lists
    virtual void clearVarLists(); // clears all varlists
    virtual void ClearMemLists(); // clears all memory lists (memhandles)
    virtual quint32 setSignalRouting(tRouting* routingtab); // set signal routing
    virtual quint32 setDsp61850SourceAdr(cETHAdress& ethadr); // set eth source adr
    virtual quint32 setDsp61850DestAdr(cETHAdress& ethadr); // set eth dest adr
    virtual quint32 setDsp61850PriorityTagged(quint32 priotag); // set entries for eth frame decoder
    virtual quint32 setDsp61850EthTypeAppId(quint32 typAppid); // same
    virtual quint32 setDsp61850EthSynchronisation(quint32 syncdata); // setting strong sync. and dataset
    virtual quint32 resetMaximum(); // clear maximum memory
    virtual quint32 triggerIntHKSK(quint32 hksk); // trigger start hksk in intlist
    virtual void addCycListItem(QString cmd); // appends new command to cyclic list
    virtual void addIntListItem(QString cmd); // same for interrupt list
    virtual cDspMeasData* getMemHandle(QString name, DSPDATA::DspSegType segtype=DSPDATA::sGlobal); // init a new memory group and return handle
    virtual void deleteDSPMemHandle(cDspMeasData* memhandle);
    virtual void addVarItem(cDspMeasData* memgroup,cDspVar* var); // append a new dsp var to memory group
    virtual quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)
    virtual quint32 deactivateInterface(); // unload ...
    virtual quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    virtual quint32 dspMemoryRead(cDspMeasData* memgroup); // reads all vars of this memorygroup
    virtual float* setVarData(cDspMeasData* memgroup, QString s); //set the values of memgroup from qstring
    virtual quint32 dspMemoryWrite(cDspMeasData* memgroup, DSPDATA::dType type); // writes all vars of this memorygroup with type
    virtual float* data(cDspMeasData* dspdata); // returns a pointer to the vars read from dsp
    virtual quint32 setGainCorrection(int chn, float val); // sets gaincorrection for 1 channel
    virtual quint32 setPhaseCorrection(int chn, float val); // sets phase correction for 1 channel
    virtual quint32 setOffsetCorrection(int chn, float val); // sets offset correction for 1 channel
    virtual quint32 readDeviceVersion(); // reads the dsp devices version
    virtual quint32 readServerVersion(); // what do you think ?

signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, QVariant answer);

private:
    Q_DECLARE_PRIVATE(cDSPInterface)
    cDSPInterfacePrivate *d_ptr;
};

}
}


#endif // DSPINTERFACE_H
