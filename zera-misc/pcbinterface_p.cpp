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
    connect(m_pClient, SIGNAL(answerAvailable(ProtobufMessage::NetMessage*)), this, SLOT(receiveAnswer(ProtobufMessage::NetMessage*)));
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


quint32 cPCBInterfacePrivate::getSamples()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SAMP:SAMP?"));
    m_MsgNrCmdList[msgnr] = getsamples;
    return msgnr;
}


void cPCBInterfacePrivate::receiveAnswer(ProtobufMessage::NetMessage *message)
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
        case getsamples:
            emit q->serverAnswer(lmsgnr, lreply, returnInt(lmsg));
            break;

        case getalias:
        case gettype:
        case getunit:
        case getrange:
        case getalias2:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;

        case getrangelist:
            emit q->serverAnswer(lmsgnr, lreply, returnStringList(lmsg));
            break;

        case geturvalue:
        case getrejection:
        case getovrejection:
        case getgaincorrection:
        case getoffsetcorrection:
        case getphasecorrection:
            emit q->serverAnswer(lmsgnr, lreply, returnDouble(lmsg));
            break;

        case isavail:
            emit q->serverAnswer(lmsgnr, lreply, returnBool(lmsg));
            break;

        case setrange:
        case resetstatus:
        case regnotifier:
        case unregnotifier:
        case pcbinterrupt:
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


