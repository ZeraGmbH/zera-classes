#include <netmessages.pb.h>

#include "secinterface_p.h"
#include "secinterface.h"

namespace Zera
{
namespace Server
{


cSECInterfacePrivate::cSECInterfacePrivate(cSECInterface *iface)
    :q_ptr(iface)
{
    m_pClient = 0;
}


void cSECInterfacePrivate::setClient(Proxy::cProxyClient *client)
{
    if (m_pClient) // we avoid multiple connections
        disconnect(m_pClient, 0, this, 0);
    m_pClient = client;
    connect(m_pClient, SIGNAL(answerAvailable(ProtobufMessage::NetMessage*)), this, SLOT(receiveAnswer(ProtobufMessage::NetMessage*)));
    connect(m_pClient, SIGNAL(tcpError(QAbstractSocket::SocketError)), this, SLOT(receiveError(QAbstractSocket::SocketError)));
}


quint32 cSECInterfacePrivate::setECalcUnit(int n)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:SET"), par = QString("%1;").arg(n));
    m_MsgNrCmdList[msgnr] = setecalcunit;
    return msgnr;
}


quint32 cSECInterfacePrivate::freeECalcUnits()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:FREE"));
    m_MsgNrCmdList[msgnr] = freeecalcunit;
    return msgnr;
}


quint32 cSECInterfacePrivate::writeRegister(QString chnname, quint8 reg, quint32 value)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:REG").arg(chnname), par = QString("%1;%2;").arg(reg).arg(value));
    m_MsgNrCmdList[msgnr] = writeregister;
    return msgnr;
}


quint32 cSECInterfacePrivate::readRegister(QString chnname, quint8 reg)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:REG?").arg(chnname), par = QString("%1;").arg(reg));
    m_MsgNrCmdList[msgnr] = readregister;
    return msgnr;
}


quint32 cSECInterfacePrivate::setSync(QString chnname, QString syncChn)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:SYNC?").arg(chnname), par = syncChn);
    m_MsgNrCmdList[msgnr] = setsync;
    return msgnr;
}


quint32 cSECInterfacePrivate::setMux(QString chnname, quint8 mux)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:MUX?").arg(chnname), par = QString("%1;").arg(mux));
    m_MsgNrCmdList[msgnr] = setmux;
    return msgnr;
}


quint32 cSECInterfacePrivate::setCmdid(QString chnname, quint8 cmdid)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:CMDID?").arg(chnname), par = QString("%1;").arg(cmdid));
    m_MsgNrCmdList[msgnr] = setcmdid;
    return msgnr;
}


quint32 cSECInterfacePrivate::start(QString chnname)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:START").arg(chnname));
    m_MsgNrCmdList[msgnr] = startecalc;
    return msgnr;
}


quint32 cSECInterfacePrivate::stop(QString chnname)
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("ECAL:%1:STOP").arg(chnname));
    m_MsgNrCmdList[msgnr] = stopecalc;
    return msgnr;
}


quint32 cSECInterfacePrivate::registerNotifier(QString query, QString notifier)
{
    QString cmd, par;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:REG"), par = QString("%1;%2;").arg(query).arg(notifier));
    m_MsgNrCmdList[msgnr] = regnotifier;
    return msgnr;
}


quint32 cSECInterfacePrivate::unregisterNotifiers()
{
    QString cmd;
    quint32 msgnr;

    msgnr = sendCommand(cmd = QString("SERV:UNR;"));
    m_MsgNrCmdList[msgnr] = unregnotifier;
    return msgnr;
}


void cSECInterfacePrivate::receiveAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        quint32 lmsgnr;
        QString lmsg = "";
        int lreply;

        lmsgnr = message->messagenr();

        if (message->reply().has_body())
        {
            lmsg = QString::fromStdString(message->reply().body());
        }

        lreply = message->reply().rtype();

        int lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cSECInterface);

        switch (lastCmd)
        {
        case setecalcunit:
        case freeecalcunit:
        case writeregister:
        case readregister:
        case setsync:
        case setmux:
        case setcmdid:
        case startecalc:
        case stopecalc:
        case regnotifier:
        case unregnotifier:
            emit q->serverAnswer(lmsgnr, lreply, returnString(lmsg));
            break;
        }
    }
}


void cSECInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cSECInterface);
    emit q->tcpError(errorCode);
}

}
}


