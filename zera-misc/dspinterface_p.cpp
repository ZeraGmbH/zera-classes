#include <netmessages.pb.h>

#include "dspinterface_p.h"
#include "dspinterface.h"

namespace Zera
{
namespace Server
{

cDSPInterfacePrivate::cDSPInterfacePrivate(cDSPInterface *iface)
    :q_ptr(iface)
{
    m_pClient = 0;
}


void cDSPInterfacePrivate::setClient(Proxy::cProxyClient *client)
{
    if (m_pClient) // we avoid multiple connections
        disconnect(m_pClient, 0, this, 0);
    m_pClient = client;
    connect(m_pClient, &Proxy::cProxyClient::answerAvailable, this, &cDSPInterfacePrivate::receiveAnswer);
    connect(m_pClient, SIGNAL(tcpError(QAbstractSocket::SocketError)), this, SLOT(receiveError(QAbstractSocket::SocketError)));
}


quint32 cDSPInterfacePrivate::bootDsp()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:BOOT:EXEC"));
    m_MsgNrCmdList[msgnr] = bootdsp;
    return msgnr;


}


quint32 cDSPInterfacePrivate::resetDsp()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:RES"));
    m_MsgNrCmdList[msgnr] = resetdsp;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDSPBootPath(QString path)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:BOOT:PATH"), path);
    m_MsgNrCmdList[msgnr] = setbootpath;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setSamplingSystem(int chncount, int samp_per, int samp_mper)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:SAMP"), par = QString("%1,%2,%3").arg(chncount).arg(samp_per).arg(samp_mper));
    m_MsgNrCmdList[msgnr] = setsamplingsystem;
    return msgnr;
}


quint32 cDSPInterfacePrivate::varList2Dsp() // the complete list has several partial lists
{
    QString vlist, cmd;
    quint32 msgnr;

    QTextStream ts(&vlist, QIODevice::WriteOnly);
    cDspMeasData* pDspMeasData;
    for (int i = 0; i < m_DspMemoryDataList.count(); i++)
    {
        pDspMeasData = m_DspMemoryDataList.at(i);
        ts << pDspMeasData->VarList(DSPDATA::vDspParam | DSPDATA::vDspTemp | DSPDATA::vDspResult | DSPDATA::vDspTempGlobal, true);
    }

    msgnr = sendCommand(cmd = QString("MEAS:LIST:RAVL"), vlist);
    m_MsgNrCmdList[msgnr] = varlist2dsp;
    return msgnr;
}


quint32 cDSPInterfacePrivate::cmdList2Dsp()
{
    QString plist, cmd;
    quint32 msgnr;

    QTextStream ts( &plist, QIODevice::WriteOnly );
    for ( QStringList::Iterator it = CycCmdList.begin(); it != CycCmdList.end(); ++it )
        ts << *it << ";" ;

    msgnr = sendCommand(cmd = QString("MEAS:LIST:CYCL"), plist);
    m_MsgNrCmdList[msgnr] = cmdlist2dsp;
    return msgnr;
}


quint32 cDSPInterfacePrivate::intList2Dsp()
{
    QString plist, cmd;
    quint32 msgnr;

    QTextStream ts( &plist, QIODevice::WriteOnly );
    for ( QStringList::Iterator it = IntCmdList.begin(); it != IntCmdList.end(); ++it )
        ts << *it << ";" ;

    msgnr = sendCommand(cmd = QString("MEAS:LIST:INTL"), plist);
    m_MsgNrCmdList[msgnr] = intlist2dsp;
    return msgnr;
}


int cDSPInterfacePrivate::cmdListCount()
{
    return CycCmdList.count();
}


int cDSPInterfacePrivate::intListCount()
{
    return IntCmdList.count();
}


void cDSPInterfacePrivate::clearCmdList()
{
    CycCmdList.clear();
    IntCmdList.clear();
}


void cDSPInterfacePrivate::clearMemLists()
{
    while ( !m_DspMemoryDataList.isEmpty() )
        delete m_DspMemoryDataList.takeFirst();
}


quint32 cDSPInterfacePrivate::setSignalRouting(tRouting *routingtab)
{
    QString cmd, par;
    quint32 msgnr;

    QTextStream ts(&par, QIODevice::WriteOnly);
    ts << "ETHROUTINGTAB";
    for (uint i = 0; i < (sizeof(tRouting)/sizeof(quint32)); i++)
        ts << "," << routingtab[i];

    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par);
    m_MsgNrCmdList[msgnr] = setsignalrouting;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850SourceAdr(cETHAdress &ethadr)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:EN61:MAC:SADR"), ethadr.getMacAdress());
    m_MsgNrCmdList[msgnr] = setdsp61850sourceadr;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850DestAdr(cETHAdress &ethadr)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:EN61:MAC:DADR"), ethadr.getMacAdress());
    m_MsgNrCmdList[msgnr] = setdsp61850destadr;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850PriorityTagged(quint32 priotag)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:EN61:PRI"), par = QString("%1").arg(priotag));
    m_MsgNrCmdList[msgnr] = setdsp61850prioritytagged;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850EthTypeAppId(quint32 typAppid)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:EN61:ETHT"), par = QString("%1").arg(typAppid));
    m_MsgNrCmdList[msgnr] = setdsp61850ethtypeappid;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setDsp61850EthSynchronisation(quint32 syncdata)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:EN61:ETHS"), par = QString("%1").arg(syncdata));
    m_MsgNrCmdList[msgnr] = setdsp61850ethsynchronisation;
    return msgnr;
}


quint32 cDSPInterfacePrivate::resetMaximum()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:MAX:RES"));
    m_MsgNrCmdList[msgnr] = resetmaximum;
    return msgnr;
}


quint32 cDSPInterfacePrivate::triggerIntHKSK(quint32 hksk)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:DSP:TRIG:INTL:HKSK"), par = QString("%1").arg(hksk));
    m_MsgNrCmdList[msgnr] = triggerinthksk;
    return msgnr;
}


void cDSPInterfacePrivate::addCycListItem(QString cmd)
{
    CycCmdList.push_back(cmd);
}


void cDSPInterfacePrivate::addIntListItem(QString cmd)
{
    IntCmdList.push_back(cmd);
}


cDspMeasData* cDSPInterfacePrivate::getMemHandle(QString name)
{
    cDspMeasData* pdmd = new cDspMeasData(name); // create a new object
    m_DspMemoryDataList.append(pdmd); // append it to the list
    return pdmd; // return handle
}


void cDSPInterfacePrivate::deleteMemHandle(cDspMeasData *memhandle)
{
    if (m_DspMemoryDataList.contains(memhandle))
    {
        int index = m_DspMemoryDataList.indexOf(memhandle);
        cDspMeasData* pdmd = m_DspMemoryDataList.takeAt(index);
        delete pdmd;
    }
}


quint32 cDSPInterfacePrivate::activateInterface()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEAS:LIST:SET"));
    m_MsgNrCmdList[msgnr] = activateinterface;
    return msgnr;
}


quint32 cDSPInterfacePrivate::deactivateInterface()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEAS:LIST:CLE"));
    m_MsgNrCmdList[msgnr] = deactivateinterface;
    return msgnr;
}


quint32 cDSPInterfacePrivate::dataAcquisition(cDspMeasData *memgroup)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEAS"), par = QString("%1").arg(memgroup->VarList(DSPDATA::vDspResult)));
    m_MsgNrCmdList[msgnr] = dataacquisition;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = DSPDATA::dFloat;
    return msgnr;
}


quint32 cDSPInterfacePrivate::dspMemoryRead(cDspMeasData *memgroup, DSPDATA::dType type)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEM:READ"), par = QString("%1").arg(memgroup->VarList(DSPDATA::vDspALL)));
    m_MsgNrCmdList[msgnr] = dspmemoryread;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = type;
    return msgnr;
}


void cDSPInterfacePrivate::setVarData(cDspMeasData *memgroup, QString datalist, DSPDATA::dType type)
{
    QStringList DataEntryList, DataList; // werte zuorden

    bool ok;

    // we expect something like Name1:0.2,0.4,0,3;Name2:1,2,3; ......

    DataEntryList = datalist.split(";"); // wir haben jetzt eine stringliste mit allen werten

    for ( QStringList::Iterator it = DataEntryList.begin(); it != DataEntryList.end(); ++it )
    {
        QString snamedata, sname, sdata;

        snamedata = *it;
        if (!snamedata.isEmpty())
        {
            sname = snamedata.section(":",0,0);
            sdata = snamedata.section(":",1,1);

            float *val = memgroup->data(sname);
            if (val != 0)
            {
                DataList = sdata.split(",");
                for ( QStringList::Iterator it2 = DataList.begin(); it2 != DataList.end(); ++it2,val++ )
                {
                    QString s;
                    s = *it2;
                    s.remove(';');
                    if (type == DSPDATA::dInt)
                    {
                        ulong vul = s.toULong(&ok); // test auf ulong
                        *((ulong*) val) = vul;
                    }
                    else
                        *val = s.toFloat();
                }
            }
        }
    }
}


quint32 cDSPInterfacePrivate::dspMemoryWrite(cDspMeasData *memgroup)
{
    quint32 msgnr;
    QString cmd, par;
    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par = memgroup->writeCommand());
    // qDebug() << QString("%1 %2").arg(cmd).arg(par);
    m_MsgNrCmdList[msgnr] = dspmemorywrite;
    return msgnr;
}


float* cDSPInterfacePrivate::data(cDspMeasData *memgroup, QString name)
{
    return memgroup->data(name);
}


void cDSPInterfacePrivate::getData(cDspMeasData *memgroup, QVector<float> &vector)
{
    vector = memgroup->getData();
}


quint32 cDSPInterfacePrivate::setGainCorrection(int chn, float val)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par = QString("GAINCORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setgaincorrection;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setPhaseCorrection(int chn, float val)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par = QString("PHASECORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setphasecorrection;
    return msgnr;
}


quint32 cDSPInterfacePrivate::setOffsetCorrection(int chn, float val)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par = QString("OFFSETCORRECTION+%1,%2").arg(chn).arg(val));
    m_MsgNrCmdList[msgnr] = setoffsetcorrection;
    return msgnr;
}


quint32 cDSPInterfacePrivate::readDeviceVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:DEV?"));
    m_MsgNrCmdList[msgnr] = readdeviceversion;
    return msgnr;
}


quint32 cDSPInterfacePrivate::readServerVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:SERV?"));
    m_MsgNrCmdList[msgnr] = readserverversion;
    return msgnr;
}


void cDSPInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply())
    {
        quint32 lmsgnr;
        QString lmsg = "";
        quint8 lreply;

        lmsgnr = message->messagenr();

        if (message->reply().has_body())
        {
            lmsg = QString::fromStdString(message->reply().body());
        }

        lreply = message->reply().rtype();
        int lastCmd;

        if (lmsgnr == 0)
            lastCmd = dspinterrupt;
        else
            lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cDSPInterface);

        switch (lastCmd)
        {
        case bootdsp:
        case resetdsp:
        case setbootpath:
        case setsamplingsystem:
        case varlist2dsp:
        case cmdlist2dsp:
        case intlist2dsp:
        case setsignalrouting:
        case setdsp61850sourceadr:
        case setdsp61850destadr:
        case setdsp61850prioritytagged:
        case setdsp61850ethtypeappid:
        case setdsp61850ethsynchronisation:
        case resetmaximum:
        case triggerinthksk:
        case activateinterface:
        case deactivateinterface:
        case dspmemorywrite:
        case setgaincorrection:
        case setphasecorrection:
        case setoffsetcorrection:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        case readdeviceversion:
        case readserverversion:
        case dspinterrupt:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        case dataacquisition:
        case dspmemoryread: {
            cDspMeasData* actMemGroup = m_MsgNrMeasData.take(lmsgnr);
            DSPDATA::dType actMemType = m_MsgNrMemType.take(lmsgnr);
            setVarData(actMemGroup, lmsg, actMemType);
            emit q->serverAnswer(lmsgnr, 0, returnString(lmsg));
            break;
        }

        }
    }
}


void cDSPInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cDSPInterface);
    emit q->tcpError(errorCode);
}

}
}
