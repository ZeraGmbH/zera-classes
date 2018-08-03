#include <netmessages.pb.h>

#include "pcbinterface_p.h"
#include "pcbinterface.h"

namespace Zera
{
namespace Server
{

cPCBInterfacePrivate::cPCBInterfacePrivate(cPCBInterface *iface)
    :q_ptr(iface)
{
    m_pClient = 0;
}


void cPCBInterfacePrivate::setClient(Proxy::cProxyClient *client)
{
    if (m_pClient) // we avoid multiple connections
        disconnect(m_pClient, 0, this, 0);

    m_pClient = client;
    connect(m_pClient, &Proxy::cProxyClient::answerAvailable, this, &cPCBInterfacePrivate::receiveAnswer);
    connect(m_pClient, SIGNAL(tcpError(QAbstractSocket::SocketError)), this, SLOT(receiveError(QAbstractSocket::SocketError)));
}


quint32 cPCBInterfacePrivate::getDSPChannel(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getdspchannel;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getStatus(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:STAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::resetStatus(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:STAT:RES;").arg(chnName));
    m_MsgNrCmdList[msgnr] = resetstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAlias(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getalias;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getType(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:TYPE?").arg(chnName));
    m_MsgNrCmdList[msgnr] = gettype;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUnit(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:UNIT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getunit;
    return  msgnr;
}


quint32 cPCBInterfacePrivate::getRange(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getrange;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRangeList(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG:CAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getrangelist;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAlias(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:ALI?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = getalias2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getType(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:TYPE?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = gettype2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUrvalue(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:URV?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = geturvalue;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUrvalue(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:URV?").arg(chnName));
    m_MsgNrCmdList[msgnr] = geturvalue2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:REJ?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = getrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getOVRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:OVR?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = getovrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getADCRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:ADCR?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = getadcrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::isAvail(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:AVA?").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = isavail;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getGainCorrection(QString chnName, QString rngName, double ampl)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:GAIN?").arg(chnName).arg(rngName), par= QString("%1;").arg(ampl));
    m_MsgNrCmdList[msgnr] = getgaincorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getOffsetCorrection(QString chnName, QString rngName, double ampl)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:OFFS?").arg(chnName).arg(rngName), par= QString("%1;").arg(ampl));
    m_MsgNrCmdList[msgnr] = getoffsetcorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPhaseCorrection(QString chnName, QString rngName, double ampl)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:PHAS?").arg(chnName).arg(rngName), par= QString("%1;").arg(ampl));
    m_MsgNrCmdList[msgnr] = getphasecorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setRange(QString chnName, QString rngName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG").arg(chnName), par = QString("%1;").arg(rngName));
    m_MsgNrCmdList[msgnr] = setrange;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setMMode(QString mmode)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:MMODE"), par = QString("%1;").arg(mmode));
    m_MsgNrCmdList[msgnr] = setmeasuringmode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustComputation()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:CORR:COMP;"));
    m_MsgNrCmdList[msgnr] = adjustcomputation;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustStorage()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:FLAS:WRIT;"));
    m_MsgNrCmdList[msgnr] = adjuststorage;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustInit(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:init;").arg(chnName).arg(rngName));
    m_MsgNrCmdList[msgnr] = adjustinit;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustGainStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:gain:stat").arg(chnName).arg(rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = setadjustgainstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustPhaseStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:phas:stat").arg(chnName).arg(rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = setadjustphasestatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustOffsetStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:offs:stat").arg(chnName).arg(rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = setadjustoffsetstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setGainNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:gain:node:%3").arg(chnName).arg(rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = setgainnode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPhaseNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:phas:node:%3").arg(chnName).arg(rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = setphasenode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setOffsetNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:offs:node:%3").arg(chnName).arg(rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = setoffsetnode;
    return msgnr;
}




quint32 cPCBInterfacePrivate::getAliasSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getaliassource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getDSPChannelSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getdspchannelsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getFormFactorSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:FFAC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getformfactorsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getConstantSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:CONS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getconstantsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setConstantSource(QString chnName, double constant)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:CONS").arg(chnName), par = QString("%1;").arg(constant));
    m_MsgNrCmdList[msgnr] = setconstantsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPowTypeSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:POWT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getpowtypesource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPowTypeSource(QString chnName, QString ptype)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:POWT").arg(chnName), par = QString("%1;").arg(ptype));
    m_MsgNrCmdList[msgnr] = setpowtypesource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasSample(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getaliassample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getSampleRate()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:SRAT?"));
    m_MsgNrCmdList[msgnr] = getsamplerate;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRangeListSample(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:RANG:CAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getrangelistsample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setRangeSample(QString chnName, QString rngName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:RANG").arg(chnName), par = QString("%1;").arg(rngName));
    m_MsgNrCmdList[msgnr] = setrangesample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPLLChannel(QString samplechnName, QString pllchnName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:PLL").arg(samplechnName), par = QString("%1;").arg(pllchnName));
    m_MsgNrCmdList[msgnr] = setpllchannel;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasSchead(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SCHEAD:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getaliassschead;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasFrqinput(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("FRQINPUT:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = getaliasfrqinput;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjustmentStatus()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("STAT:ADJ?"));
    m_MsgNrCmdList[msgnr] = getadjustmentstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPCBErrorStatus()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("STAT:PCB:ERR?"));
    m_MsgNrCmdList[msgnr] = getpcberrorstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjustmentChksum()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:FLAS:CHKS?"));
    m_MsgNrCmdList[msgnr] = getadjustmentchksum;
    return msgnr;
}

quint32 cPCBInterfacePrivate::resourceAliasQuery(QString resourceType, QString resourceName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("%1:%2:ALI?").arg(resourceType).arg(resourceName));
    m_MsgNrCmdList[msgnr] = resourcealiasquery;
    return msgnr;
}


quint32 cPCBInterfacePrivate::registerNotifier(QString query, QString notifier)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:REG"), par = QString("%1;%2;").arg(query).arg(notifier));
    m_MsgNrCmdList[msgnr] = regnotifier;
    return msgnr;
}


quint32 cPCBInterfacePrivate::unregisterNotifiers()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:UNR;"));
    m_MsgNrCmdList[msgnr] = unregnotifier;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readServerVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:SERV?"));
    m_MsgNrCmdList[msgnr] = getserverversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readPCBVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:PCB?"));
    m_MsgNrCmdList[msgnr] = getpcbversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readFPGAVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:FPGA?"));
    m_MsgNrCmdList[msgnr] = getfpgaversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readCTRLVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:CTRL?"));
    m_MsgNrCmdList[msgnr] = getctrlversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readSerialNr()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:SER?"));
    m_MsgNrCmdList[msgnr] = getserialnumber;
    return msgnr;
}


void cPCBInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply())
    {
        quint32 lmsgnr;
        QString lmsg = "";
        int lreply = 0;

        lmsgnr = message->messagenr();

        if (message->reply().has_body())
        {
            lmsg = QString::fromStdString(message->reply().body());
        }

        lreply = message->reply().rtype();


        int lastCmd;

        if (lmsgnr == 0)
            lastCmd = pcbinterrupt;
        else
            lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cPCBInterface);

        switch (lastCmd)
        {
        case getdspchannel:
        case getstatus:
        case gettype2:
        case getsamplerate:
        case getdspchannelsource:
        case getadjustmentstatus:
            emit q->serverAnswer(lmsgnr, lreply, returnInt(lmsg));
            break;

        case getalias:
        case gettype:
        case getunit:
        case getrange:
        case getalias2:
        case getaliassource:
        case getaliassample:
        case getaliassschead:
        case getaliasfrqinput:
        case resourcealiasquery:
        case getserverversion:
        case getpcbversion:
        case getfpgaversion:
        case getctrlversion:
        case getserialnumber:
        case getadjustmentchksum:
        case getpcberrorstatus:
        case getpowtypesource:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;

        case getrangelist:
        case getrangelistsample:
            emit q->serverAnswer(lmsgnr, lreply, returnStringList(lmsg));
            break;

        case geturvalue:
        case geturvalue2:
        case getrejection:
        case getovrejection:
        case getadcrejection:
        case getgaincorrection:
        case getoffsetcorrection:
        case getphasecorrection:
        case getformfactorsource:
        case getconstantsource:
            emit q->serverAnswer(lmsgnr, lreply, returnDouble(lmsg));
            break;

        case isavail:
            emit q->serverAnswer(lmsgnr, lreply, returnBool(lmsg));
            break;

        case setoffsetnode:
        case setphasenode:
        case setgainnode:
        case adjuststorage:
        case adjustinit:
        case setadjustgainstatus:
        case setadjustphasestatus:
        case setadjustoffsetstatus:
        case adjustcomputation:
        case setmeasuringmode:
        case setrange:
        case resetstatus:
        case regnotifier:
        case unregnotifier:
        case pcbinterrupt:
        case setrangesample:
        case setpllchannel:
        case setconstantsource:
        case setpowtypesource:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        }
    } // hmm ... we have to look what to do otherwise
}


void cPCBInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cPCBInterface);
    emit q->tcpError(errorCode);
}


}
}


