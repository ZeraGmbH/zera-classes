#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <QObject>

#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <netmessages.pb.h>

#include "pcbproxy_global.h"
#include "interface.h"

namespace Zera
{
namespace PCBProxy
{


class cInterfacePrivate: public cInterface
{

   Q_OBJECT

public:
    cInterfacePrivate(QObject* parent = 0);
    virtual void getDSPChannel(QString chnName); // int
    virtual void getStatus(QString chnName); // int
    virtual void getAlias(QString chnName); // qstring
    virtual void getType(QString chnName); // qstring
    virtual void getUnit(QString chnName); // qstring
    virtual void getRange(QString chnName); // qstring
    virtual void getRangeList(QString chnName); // qstringlist
    virtual void getAlias(QString chnName, QString rngName); // qstring
    virtual void getType(QString chnName, QString rngName); // int
    virtual void getUrvalue(QString chnName, QString rngName); // double
    virtual void getRejection(QString chnName, QString rngName); // double
    virtual void getOVRejection(QString chnName, QString rngName); // double
    virtual void isAvail(QString chnName, QString rngName); // bool

    virtual void setRange(QString chnName, QString rngName); // reply (ack, nak..)

    virtual void transferAnswer(ProtobufMessage::NetMessage *message);

signals:
    void pcbCommand(ProtobufMessage::NetMessage *message);
    void pcbAnswerAvail();
    void pcbReplyAvail();

private:
    QString m_sMessage;
    int m_nReply;

    QStateMachine getDSPChannelStatemachine;
    QState getDSPChannelStateStart;
    QFinalState getDSPChannelStateFinish;

    QStateMachine getStatusStatemachine;
    QState getStatusStateStart;
    QFinalState getStatusStateFinish;

    QStateMachine getAliasStatemachine;
    QState getAliasStateStart;
    QFinalState getAliasStateFinish;

    QStateMachine getTypeStatemachine;
    QState getTypeStateStart;
    QFinalState getTypeStateFinish;

    QStateMachine getUnitStatemachine;
    QState getUnitStateStart;
    QFinalState getUnitStateFinish;

    QStateMachine getRangeStatemachine;
    QState getRangeStateStart;
    QFinalState getRangeStateFinish;

    QStateMachine getRangeListStatemachine;
    QState getRangeListStateStart;
    QFinalState getRangeListStateFinish;

    QStateMachine getRngAliasStatemachine;
    QState getRngAliasStateStart;
    QFinalState getRngAliasStateFinish;

    QStateMachine getRngTypeStatemachine;
    QState getRngTypeStateStart;
    QFinalState getRngTypeStateFinish;

    QStateMachine getUrvalueStatemachine;
    QState getUrvalueStateStart;
    QFinalState getUrvalueStateFinish;

    QStateMachine getRejectionStatemachine;
    QState getRejectionStateStart;
    QFinalState getRejectionStateFinish;

    QStateMachine getOVRejectionStatemachine;
    QState getOVRejectionStateStart;
    QFinalState getOVRejectionStateFinish;

    QStateMachine isAvailStatemachine;
    QState isAvailStateStart;
    QFinalState isAvailStateFinish;

    QStateMachine setRangeStatemachine;
    QState setRangeStateStart;
    QFinalState setRangeStateFinish;

    void sendCommand(QString cmd);
    void sendCommand(QString cmd, QString par);

private slots:
    void returnInt();
    void returnString();
    void returnStringList();
    void returnDouble();
    void returnBool();
    void returnReply();

};

}
}

#endif // INTERFACE_P_H
