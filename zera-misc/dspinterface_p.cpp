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
}


void cDSPInterfacePrivate::setClient(Proxi::cProxiClient *client)
{
    m_pClient = client;
    connect(m_pClient, SIGNAL(answerAvailable(ProtobufMessage::NetMessage*)), this, SLOT(receiveAnswer(ProtobufMessage::NetMessage*)));
    connect(m_pClient, SIGNAL(tcpError(QAbstractSocket::SocketError errorCode)), this, SLOT(receiveError(QAbstractSocket::SocketError)));
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
    for (int i = 0; i < m_DspMeasDataList.count(); i++)
    {
        pDspMeasData = m_DspMeasDataList.at(i);
        ts << pDspMeasData->VarList();
    }

    msgnr = sendCommand(cmd = QString("MEAS:LIST:RAVL"), vlist);
    m_MsgNrCmdList[msgnr] = varlist2dsp;
    return msgnr;
}


quint32 cDSPInterfacePrivate::cmdLists2Dsp()
{
    QString plist, cmd;
    quint32 msgnr;

    QTextStream ts( &plist, QIODevice::WriteOnly );
    for ( QStringList::Iterator it = CycCmdList.begin(); it != CycCmdList.end(); ++it )
        ts << *it << ";" ;

    msgnr = sendCommand(cmd = QString("MEAS:LIST:CYCL"), plist);
    m_MsgNrCmdList[msgnr] = cmdlists2dsp;
    return msgnr;
}


void cDSPInterfacePrivate::clearCmdList()
{
    CycCmdList.clear();
    IntCmdList.clear();
}


void cDSPInterfacePrivate::clearVarLists()
{
    while ( !m_DspMeasDataList.isEmpty())
        delete m_DspMeasDataList.takeFirst();
}


void cDSPInterfacePrivate::ClearMemLists()
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


cDspMeasData* cDSPInterfacePrivate::getMemHandle(QString name, DSPDATA::DspSegType segtype)
{
    cDspMeasData* pdmd = new cDspMeasData(name, segtype); // create a new object
    m_DspMemoryDataList.append(pdmd); // append it to the list
    return pdmd; // return handle
}


void cDSPInterfacePrivate::addVarItem(cDspMeasData *memgroup, cDspVar *var)
{
    memgroup->addVarItem(var);
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

    msgnr = sendCommand(cmd = QString("MEAS"), par = QString("%1").arg(memgroup->MeasVarList()));
    m_MsgNrCmdList[msgnr] = dataacquisition;
    return msgnr;
}


quint32 cDSPInterfacePrivate::dspMemoryRead(cDspMeasData *memgroup)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("MEM:READ"), par = QString("%1").arg(memgroup->VarList()));
    m_MsgNrCmdList[msgnr] = dspmemoryread;
    return msgnr;
}


float* cDSPInterfacePrivate::setVarData(cDspMeasData *memgroup, QString data)
{
    QStringList DataEntryList, DataList; // werte zuorden
    QString s;
    bool ok;

    DataEntryList = data.split(";"); // wir haben jetzt eine stringliste mit allen werten
    float *val = memgroup->data();
    for ( QStringList::Iterator it = DataEntryList.begin(); it != DataEntryList.end(); ++it )
    {
        s = *it;
        s = s.section(":",1,1);
        DataList = s.split(",");
        for ( QStringList::Iterator it2 = DataList.begin(); it2 != DataList.end(); ++it2,val++ )
        {
            s = *it2;
            s.remove(';');
            ulong vul = s.toULong(&ok); // test auf ulong
            if (ok)
                *((ulong*) val) = vul;
            else
                *val = s.toFloat();
        }
    }
    return memgroup->data();
}


quint32 cDSPInterfacePrivate::dspMemoryWrite(cDspMeasData *memgroup, DSPDATA::dType type)
{
    QString list;
    QStringList DataEntryList;
    QString s, cmd, par;
    quint32 msgnr;
    QTextStream ts(&par, QIODevice::WriteOnly );

    list = memgroup->VarList(); // liste mit allen variablen und deren länge
    float* fval = memgroup->data();
    ulong* lval = (ulong*) fval;
    DataEntryList = list.split(";"); // wir haben jetzt eine stringliste mit je variable, länge
    for ( QStringList::Iterator it = DataEntryList.begin(); it != DataEntryList.end(); ++it )
    {
        s = *it; // einen eintrag variable, länge
        ts << s.section(",",0,0); // den namen,
        int n = s.section(",",1,1).toInt(); // anzahl der werte für diese var.
        for (int i = 0;i < n; i++)
        {
            ts << ",";
            if (type == DSPDATA::dInt)  // wir haben integer daten
            {
                ts << *lval;
                lval++;
            }
            else
            {
                ts << *fval;
                fval++;
            }
        }
        ts << ";";
    }

    msgnr = sendCommand(cmd = QString("MEM:WRIT"), par);
    m_MsgNrCmdList[msgnr] = dspmemorywrite;
    return msgnr;
}


float* cDSPInterfacePrivate::data(cDspMeasData *dspdata)
{
    return dspdata->data();
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


void cDSPInterfacePrivate::receiveAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        quint32 lmsgnr;
        QString lmsg;
        int lreply = 0;

        lmsgnr = message->messagenr();

        if (message->reply().has_body())
        {
            lmsg = QString::fromStdString(message->reply().body());
        }
        else
        {
            lreply = message->reply().rtype();
        }

        int lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cDSPInterface);

        switch (lastCmd)
        {
        case bootdsp:
        case resetdsp:
        case setbootpath:
        case setsamplingsystem:
        case varlist2dsp:
        case cmdlists2dsp:
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
            emit q->serverAnswer(lmsgnr, returnReply(lreply));
            break;
        case dataacquisition:
        case dspmemoryread:
        case readdeviceversion:
        case readserverversion:
            emit q->serverAnswer(lmsgnr, returnString(lmsg));
            break;
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
