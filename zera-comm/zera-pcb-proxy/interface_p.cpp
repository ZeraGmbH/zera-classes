#include <QStringList>

#include "interface_p.h"


namespace Zera
{
namespace PCBProxy
{


cInterfacePrivate::cInterfacePrivate(QObject *parent)
{
    setParent(parent);

    getDSPChannelStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getDSPChannelStateFinish);
    getDSPChannelStatemachine.addState(&getDSPChannelStateStart);
    getDSPChannelStatemachine.addState(&getDSPChannelStateFinish);
    getDSPChannelStatemachine.setInitialState(&getDSPChannelStateStart);
    QObject::connect(&getDSPChannelStateFinish, SIGNAL(entered()), this, SLOT(returnInt()));

    getStatusStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getStatusStateFinish);
    getStatusStatemachine.addState(&getStatusStateStart);
    getStatusStatemachine.addState(&getStatusStateFinish);
    getStatusStatemachine.setInitialState(&getStatusStateStart);
    QObject::connect(&getStatusStateFinish, SIGNAL(entered()), this, SLOT(returnInt()));

    getAliasStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getAliasStateFinish);
    getAliasStatemachine.addState(&getAliasStateStart);
    getAliasStatemachine.addState(&getAliasStateFinish);
    getAliasStatemachine.setInitialState(&getAliasStateStart);
    QObject::connect(&getAliasStateFinish, SIGNAL(entered()), this, SLOT(returnString()));

    getTypeStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getTypeStateFinish);
    getTypeStatemachine.addState(&getTypeStateStart);
    getTypeStatemachine.addState(&getTypeStateFinish);
    getTypeStatemachine.setInitialState(&getTypeStateStart);
    QObject::connect(&getTypeStateFinish, SIGNAL(entered()), this, SLOT(returnInt()));

    getUnitStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getUnitStateFinish);
    getUnitStatemachine.addState(&getUnitStateStart);
    getUnitStatemachine.addState(&getUnitStateFinish);
    getUnitStatemachine.setInitialState(&getUnitStateStart);
    QObject::connect(&getUnitStateFinish, SIGNAL(entered()), this, SLOT(returnString()));

    getRangeStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getRangeStateFinish);
    getRangeStatemachine.addState(&getRangeStateStart);
    getRangeStatemachine.addState(&getRangeStateFinish);
    getRangeStatemachine.setInitialState(&getRangeStateStart);
    QObject::connect(&getRangeStateFinish, SIGNAL(entered()), this, SLOT(returnString()));

    getRangeListStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getRangeListStateFinish);
    getRangeListStatemachine.addState(&getRangeListStateStart);
    getRangeListStatemachine.addState(&getRangeListStateFinish);
    getRangeListStatemachine.setInitialState(&getRangeListStateStart);
    QObject::connect(&getRangeListStateFinish, SIGNAL(entered()), this, SLOT(returnStringList()));

    getRngAliasStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getRngAliasStateFinish);
    getRngAliasStatemachine.addState(&getRngAliasStateStart);
    getRngAliasStatemachine.addState(&getRngAliasStateFinish);
    getRngAliasStatemachine.setInitialState(&getRngAliasStateStart);
    QObject::connect(&getRngAliasStateFinish, SIGNAL(entered()), this, SLOT(returnString()));

    getRngTypeStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getRngTypeStateFinish);
    getRngTypeStatemachine.addState(&getRngTypeStateStart);
    getRngTypeStatemachine.addState(&getRngTypeStateFinish);
    getRngTypeStatemachine.setInitialState(&getRngTypeStateStart);
    QObject::connect(&getRngTypeStateFinish, SIGNAL(entered()), this, SLOT(returnInt()));

    getUrvalueStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getUrvalueStateFinish);
    getUrvalueStatemachine.addState(&getUrvalueStateStart);
    getUrvalueStatemachine.addState(&getUrvalueStateFinish);
    getUrvalueStatemachine.setInitialState(&getUrvalueStateStart);
    QObject::connect(&getUrvalueStateFinish, SIGNAL(entered()), this, SLOT(returnDouble()));

    getRejectionStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getRejectionStateFinish);
    getRejectionStatemachine.addState(&getRejectionStateStart);
    getRejectionStatemachine.addState(&getRejectionStateFinish);
    getRejectionStatemachine.setInitialState(&getRejectionStateStart);
    QObject::connect(&getRejectionStateFinish, SIGNAL(entered()), this, SLOT(returnDouble()));

    getOVRejectionStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &getOVRejectionStateFinish);
    getOVRejectionStatemachine.addState(&getOVRejectionStateStart);
    getOVRejectionStatemachine.addState(&getOVRejectionStateFinish);
    getOVRejectionStatemachine.setInitialState(&getOVRejectionStateStart);
    QObject::connect(&getOVRejectionStateFinish, SIGNAL(entered()), this, SLOT(returnDouble()));

    isAvailStateStart.addTransition(this, SIGNAL(pcbAnswerAvail()), &isAvailStateFinish);
    isAvailStatemachine.addState(&isAvailStateStart);
    isAvailStatemachine.addState(&isAvailStateFinish);
    isAvailStatemachine.setInitialState(&isAvailStateStart);
    QObject::connect(&isAvailStateFinish, SIGNAL(entered()), this, SLOT(returnBool()));

    setRangeStateStart.addTransition(this, SIGNAL(pcbReplyAvail()), &setRangeStateFinish);
    setRangeStatemachine.addState(&setRangeStateStart);
    setRangeStatemachine.addState(&setRangeStateFinish);
    setRangeStatemachine.setInitialState(&setRangeStateStart);
    QObject::connect(&setRangeStateFinish, SIGNAL(entered()), this, SLOT(returnReply()));

}


void cInterfacePrivate::getDSPChannel(QString chnName)
{
    QString cmd;

    getDSPChannelStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:DSPC?").arg(chnName));
}


void cInterfacePrivate::getStatus(QString chnName)
{
    QString cmd;

    getStatusStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:STAT?").arg(chnName));
}


void cInterfacePrivate::getAlias(QString chnName)
{
    QString cmd;

    getAliasStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:ALI?").arg(chnName));
}


void cInterfacePrivate::getType(QString chnName)
{
    QString cmd;

    getTypeStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:TYPE?").arg(chnName));
}


void cInterfacePrivate::getUnit(QString chnName)
{
    QString cmd;

    getUnitStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:UNIT?").arg(chnName));
}


void cInterfacePrivate::getRange(QString chnName)
{
    QString cmd;

    getRangeStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:RANG?").arg(chnName));
}


void cInterfacePrivate::getRangeList(QString chnName)
{
    QString cmd;

    getRangeListStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:RANG:CAT?").arg(chnName));
}


void cInterfacePrivate::getAlias(QString chnName, QString rngName)
{
    QString cmd;

    getRngAliasStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:ALI?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::getType(QString chnName, QString rngName)
{
    QString cmd;

    getRngTypeStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:TYPE?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::getUrvalue(QString chnName, QString rngName)
{
    QString cmd;

    getUrvalueStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:URV?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::getRejection(QString chnName, QString rngName)
{
    QString cmd;

    getRejectionStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:REJ?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::getOVRejection(QString chnName, QString rngName)
{
    QString cmd;

    getOVRejectionStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:OVR?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::isAvail(QString chnName, QString rngName)
{
    QString cmd;

    isAvailStatemachine.start();
    sendCommand(cmd = QString("SENS:%1:%2:AVA?").arg(chnName).arg(rngName));
}


void cInterfacePrivate::setRange(QString chnName, QString rngName)
{
    QString cmd;

    setRangeStatemachine.start();
    sendCommand(cmd = QString("SENS:%1,RANG").arg(chnName), rngName);
}


void cInterfacePrivate::transferAnswer(ProtobufMessage::NetMessage *message)
{
    if (message->has_reply())
    {
        if (message->reply().has_body())
        {
            m_sMessage = QString::fromStdString(message->reply().body());
            emit pcbAnswerAvail();
        }
        else
        {
            m_nReply = message->reply().rtype();
            emit pcbReplyAvail();
        }
    }
}


void cInterfacePrivate::sendCommand(QString cmd)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    emit pcbCommand(&envelope);
}


void cInterfacePrivate::sendCommand(QString cmd, QString par)
{
    ProtobufMessage::NetMessage envelope;
    ProtobufMessage::NetMessage::ScpiCommand* message = envelope.mutable_scpi();

    message->set_command(cmd.toStdString());
    message->set_parameter(par.toStdString());
    emit pcbCommand(&envelope);
}


void cInterfacePrivate::returnInt()
{
    bool ok;
    qint32 n;
    QVariant var;

    n = m_sMessage.toInt(&ok);
    var = QVariant(n);
    emit pcbAnswer(var);
}


void cInterfacePrivate::returnString()
{
    QVariant var;
    var = QVariant(m_sMessage);
    emit pcbAnswer(var);
}


void cInterfacePrivate::returnStringList()
{
    QVariant var;
    QString sep = ";";

    var = QVariant(m_sMessage.split(sep));
    emit pcbAnswer(var);
}


void cInterfacePrivate::returnDouble()
{
    bool ok;
    double d;
    QVariant var;

    d = m_sMessage.toDouble(&ok);
    var = QVariant(d);
    emit pcbAnswer(var);
}


void cInterfacePrivate::returnBool()
{
    bool ok, avail;
    QVariant var;

    avail = (m_sMessage.toInt(&ok) == 1);
    var = QVariant(avail);
    emit pcbAnswer(var);
}


void cInterfacePrivate::returnReply()
{
    emit pcbReply(replies(m_nReply));
}

}
}

