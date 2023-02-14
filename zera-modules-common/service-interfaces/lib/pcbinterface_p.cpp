#include "pcbinterface_p.h"
#include "pcbinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cPCBInterfacePrivate::cPCBInterfacePrivate(cPCBInterface *iface)
    :q_ptr(iface)
{
    m_pClient = nullptr;
}


void cPCBInterfacePrivate::setClient(Zera::ProxyClient *client)
{
    if (m_pClient) { // we avoid multiple connections
        disconnect(m_pClient, nullptr, this, nullptr);
    }

    m_pClient = client;
    connect(m_pClient, &Zera::ProxyClient::answerAvailable, this, &cPCBInterfacePrivate::receiveAnswer);
    connect(m_pClient, &Zera::ProxyClient::tcpError, this, &cPCBInterfacePrivate::receiveError);
}

void cPCBInterfacePrivate::setClientSmart(Zera::ProxyClientPtr client)
{
    m_clientSmart = client;
    setClient(client.get());
}


quint32 cPCBInterfacePrivate::getDSPChannel(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getdspchannel;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getStatus(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:STAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::resetStatus(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:STAT:RES;").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::resetstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAlias(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getalias;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getType(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:TYPE?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::gettype;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUnit(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:UNIT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getunit;
    return  msgnr;
}


quint32 cPCBInterfacePrivate::getRange(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getrange;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRangeList(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG:CAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getrangelist;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAlias(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:ALI?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getalias2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getType(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:TYPE?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::gettype2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUrvalue(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:URV?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::geturvalue;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getUrvalue(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:URV?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::geturvalue2;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:REJ?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getOVRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:OVR?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getovrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getADCRejection(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:ADCR?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::getadcrejection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::isAvail(QString chnName, QString rngName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:AVA?").arg(chnName, rngName));
    m_MsgNrCmdList[msgnr] = PCB::isavail;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getGainCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:GAIN?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getgaincorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjGainCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:ADJG?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getgaincorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getOffsetCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:OFFS?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getoffsetcorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjOffsetCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:ADJOFFSET?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getoffsetcorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPhaseCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:PHAS?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getphasecorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjPhaseCorrection(QString chnName, QString rngName, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:CORR:ADJP?").arg(chnName, rngName), par= QString("%1;").arg(at));
    m_MsgNrCmdList[msgnr] = PCB::getphasecorrection;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setRange(QString chnName, QString rngName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:RANG").arg(chnName), par = QString("%1;").arg(rngName));
    m_MsgNrCmdList[msgnr] = PCB::setrange;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setMMode(QString mmode)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:MMODE"), par = QString("%1;").arg(mmode));
    m_MsgNrCmdList[msgnr] = PCB::setmeasuringmode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustComputation()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:CORR:COMP;"));
    m_MsgNrCmdList[msgnr] = PCB::adjustcomputation;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustStorage()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:FLAS:WRIT;"));
    m_MsgNrCmdList[msgnr] = PCB::adjuststorage;
    return msgnr;
}


quint32 cPCBInterfacePrivate::adjustStorageClamp()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:CLAM:WRIT;"));
    m_MsgNrCmdList[msgnr] = PCB::adjuststorageclamp;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustGainStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:gain:stat").arg(chnName, rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustgainstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustPhaseStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:phas:stat").arg(chnName, rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustphasestatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setAdjustOffsetStatus(QString chnName, QString rngName, int stat)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:offs:stat").arg(chnName, rngName),
                        par = QString("%1;").arg(stat));
    m_MsgNrCmdList[msgnr] = PCB::setadjustoffsetstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setGainNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:gain:node:%3").arg(chnName, rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setgainnode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPhaseNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:phas:node:%3").arg(chnName, rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setphasenode;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setOffsetNode(QString chnName, QString rngName, int nr, double corr, double at)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SENS:%1:%2:corr:offs:node:%3").arg(chnName, rngName).arg(nr),
                        par = QString("%1;%2;").arg(corr).arg(at));
    m_MsgNrCmdList[msgnr] = PCB::setoffsetnode;
    return msgnr;
}




quint32 cPCBInterfacePrivate::getAliasSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:ALI?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliassource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getDSPChannelSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:DSPC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getdspchannelsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getFormFactorSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:FFAC?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getformfactorsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getConstantSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:CONS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getconstantsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setConstantSource(QString chnName, double constant)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:CONS").arg(chnName), par = QString("%1;").arg(constant));
    m_MsgNrCmdList[msgnr] = PCB::setconstantsource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPowTypeSource(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:POWT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getpowtypesource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPowTypeSource(QString chnName, QString ptype)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SOUR:%1:POWT").arg(chnName), par = QString("%1;").arg(ptype));
    m_MsgNrCmdList[msgnr] = PCB::setpowtypesource;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasSample(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliassample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getSampleRate()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:SRAT?"));
    m_MsgNrCmdList[msgnr] = PCB::getsamplerate;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getRangeListSample(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:RANG:CAT?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getrangelistsample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setRangeSample(QString chnName, QString rngName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:RANG").arg(chnName), par = QString("%1;").arg(rngName));
    m_MsgNrCmdList[msgnr] = PCB::setrangesample;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPLLChannel(QString samplechnName, QString pllchnName)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:%1:PLL").arg(samplechnName), par = QString("%1;").arg(pllchnName));
    m_MsgNrCmdList[msgnr] = PCB::setpllchannel;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasSchead(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SCHEAD:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliassschead;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAliasFrqinput(QString chnName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("FRQINPUT:%1:ALIAS?").arg(chnName));
    m_MsgNrCmdList[msgnr] = PCB::getaliasfrqinput;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjustmentStatus()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("STAT:ADJ?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustmentstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAuthorizationStatus()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("STAT:AUTH?"));
    m_MsgNrCmdList[msgnr] = PCB::getauthorizationstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPCBErrorStatus()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("STAT:PCB:ERR?"));
    m_MsgNrCmdList[msgnr] = PCB::getpcberrorstatus;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getAdjustmentChksum()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:FLAS:CHKS?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustmentchksum;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getPCBAdjustmentData()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:XML?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustpcbxml;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setPCBAdjustmentData(QString xmlpcb)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:XML"), par = xmlpcb);
    m_MsgNrCmdList[msgnr] = PCB::setadjustpcbxml;
    return msgnr;
}


quint32 cPCBInterfacePrivate::getClampAdjustmentData()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:CLAM:XML?"));
    m_MsgNrCmdList[msgnr] = PCB::getadjustclampxml;
    return msgnr;
}


quint32 cPCBInterfacePrivate::setClampAdjustmentData(QString xmlclamp)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:ADJ:CLAM:XML"), par = xmlclamp);
    m_MsgNrCmdList[msgnr] = PCB::setadjustclampxml;
    return msgnr;
}


quint32 cPCBInterfacePrivate::transparentCommand(QString cmd)
{
    quint32 msgnr;
    QList<QString> sl;

    sl = cmd.split(' ');
    if (sl.count() <= 1)
        msgnr = sendCommand(cmd);
    else
        msgnr = sendCommand(sl.at(0), sl.at(1));

    m_MsgNrCmdList[msgnr] = PCB::transparentcommand;
    return msgnr;
}


quint32 cPCBInterfacePrivate::resourceAliasQuery(QString resourceType, QString resourceName)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("%1:%2:ALI?").arg(resourceType).arg(resourceName));
    m_MsgNrCmdList[msgnr] = PCB::resourcealiasquery;
    return msgnr;
}


quint32 cPCBInterfacePrivate::registerNotifier(QString query, int notifyId)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:REG"), par = QString("%1;%2;").arg(query).arg(notifyId));
    m_MsgNrCmdList[msgnr] = PCB::regnotifier;
    return msgnr;
}


quint32 cPCBInterfacePrivate::unregisterNotifiers()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:UNR;"));
    m_MsgNrCmdList[msgnr] = PCB::unregnotifier;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readServerVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:SERV?"));
    m_MsgNrCmdList[msgnr] = PCB::getserverversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readPCBVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:PCB?"));
    m_MsgNrCmdList[msgnr] = PCB::getpcbversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readFPGAVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:FPGA?"));
    m_MsgNrCmdList[msgnr] = PCB::getfpgaversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readCTRLVersion()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:VERS:CTRL?"));
    m_MsgNrCmdList[msgnr] = PCB::getctrlversion;
    return msgnr;
}


quint32 cPCBInterfacePrivate::readSerialNr()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:SER?"));
    m_MsgNrCmdList[msgnr] = PCB::getserialnumber;
    return msgnr;
}


quint32 cPCBInterfacePrivate::writeSerialNr(QString serNr)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SYST:SER"), par = QString("%1;").arg(serNr));
    m_MsgNrCmdList[msgnr] = PCB::setserialnumber;
    return msgnr;
}


void cPCBInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    TAnswerDecoded decodedAnswer;
    if(decodeProtobuffAnswer(message, decodedAnswer, PCB::pcbinterrupt)) {
        Q_Q(cPCBInterface);
        switch (decodedAnswer.cmdSendEnumVal)
        {
        case PCB::getdspchannel:
        case PCB::getstatus:
        case PCB::gettype2:
        case PCB::getsamplerate:
        case PCB::getdspchannelsource:
        case PCB::getadjustmentstatus:
        case PCB::getauthorizationstatus:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnInt(decodedAnswer.msgBody));
            break;

        case PCB::getalias:
        case PCB::gettype:
        case PCB::getunit:
        case PCB::getrange:
        case PCB::getalias2:
        case PCB::getaliassource:
        case PCB::getaliassample:
        case PCB::getaliassschead:
        case PCB::getaliasfrqinput:
        case PCB::resourcealiasquery:
        case PCB::getserverversion:
        case PCB::getpcbversion:
        case PCB::getfpgaversion:
        case PCB::getctrlversion:
        case PCB::getserialnumber:
        case PCB::getadjustmentchksum:
        case PCB::getadjustpcbxml:
        case PCB::getadjustclampxml:
        case PCB::getpcberrorstatus:
        case PCB::getpowtypesource:
        case PCB::transparentcommand:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
            break;

        case PCB::getrangelist:
        case PCB::getrangelistsample:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnStringList(decodedAnswer.msgBody));
            break;

        case PCB::geturvalue:
        case PCB::geturvalue2:
        case PCB::getrejection:
        case PCB::getovrejection:
        case PCB::getadcrejection:
        case PCB::getgaincorrection:
        case PCB::getoffsetcorrection:
        case PCB::getphasecorrection:
        case PCB::getformfactorsource:
        case PCB::getconstantsource:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnDouble(decodedAnswer.msgBody));
            break;

        case PCB::isavail:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnBool(decodedAnswer.msgBody));
            break;

        case PCB::setoffsetnode:
        case PCB::setphasenode:
        case PCB::setgainnode:
        case PCB::adjuststorage:
        case PCB::adjuststorageclamp:
        case PCB::adjustinit:
        case PCB::setadjustgainstatus:
        case PCB::setadjustphasestatus:
        case PCB::setadjustoffsetstatus:
        case PCB::adjustcomputation:
        case PCB::setmeasuringmode:
        case PCB::setrange:
        case PCB::resetstatus:
        case PCB::regnotifier:
        case PCB::unregnotifier:
        case PCB::pcbinterrupt:
        case PCB::setrangesample:
        case PCB::setpllchannel:
        case PCB::setconstantsource:
        case PCB::setpowtypesource:
        case PCB::setadjustpcbxml:
        case PCB::setadjustclampxml:
        case PCB::setserialnumber:
            emit q->serverAnswer(decodedAnswer.msgNr, decodedAnswer.reply, VariantConverter::returnString(decodedAnswer.msgBody));
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

