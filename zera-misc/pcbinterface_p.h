#ifndef PCBINTERFACE_P_H
#define PCBINTERFACE_P_H

#include <QObject>

#include "interface_p.h"

namespace Zera
{
namespace Server
{

enum pcbcommands
{
    getdspchannel,
    getstatus,
    getalias,
    gettype,
    getunit,
    getrange,
    getrangelist,
    getalias2,
    gettype2,
    geturvalue,
    getrejection,
    getovrejection,
    isavail,
    getgaincorrection,
    getoffsetcorrection,
    getphasecorrection,
    setrange
};


class cPCBInterface;

class cPCBInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT

public:
    cPCBInterfacePrivate(cPCBInterface* iface);
    virtual void setClient(Zera::Proxi::cProxiClient *client);
    virtual quint32 getDSPChannel(QString chnName); // int
    virtual quint32 getStatus(QString chnName); // int
    virtual quint32 getAlias(QString chnName); // qstring
    virtual quint32 getType(QString chnName); // qstring
    virtual quint32 getUnit(QString chnName); // qstring
    virtual quint32 getRange(QString chnName); // qstring
    virtual quint32 getRangeList(QString chnName); // qstringlist
    virtual quint32 getAlias(QString chnName, QString rngName); // qstring
    virtual quint32 getType(QString chnName, QString rngName); // int
    virtual quint32 getUrvalue(QString chnName, QString rngName); // double
    virtual quint32 getRejection(QString chnName, QString rngName); // double
    virtual quint32 getOVRejection(QString chnName, QString rngName); // double
    virtual quint32 isAvail(QString chnName, QString rngName); // bool
    virtual quint32 getGainCorrection(QString chnName, QString rngName, double ampl); // double
    virtual quint32 getOffsetCorrection(QString chnName, QString rngName, double ampl); // double
    virtual quint32 getPhaseCorrection(QString chnName, QString rngName, double ampl); // double

    virtual quint32 setRange(QString chnName, QString rngName); // reply (ack, nak..)

protected slots:
    virtual void receiveAnswer(ProtobufMessage::NetMessage *message);
    virtual void receiveError(QAbstractSocket::SocketError errorCode);

private:
    Q_DECLARE_PUBLIC(cPCBInterface)
    cPCBInterface *q_ptr;
};

}
}

#endif // PCBINTERFACE_P_H
