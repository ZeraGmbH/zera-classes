#ifndef PCBINTERFACE_H
#define PCBINTERFACE_H

#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

#include "zera_misc_global.h"

namespace Zera
{
namespace Proxy
{
    class cProxyClient;
}
}


namespace Zera
{
namespace Server
{

class cPCBInterfacePrivate;


class ZERA_MISCSHARED_EXPORT cPCBInterface: public QObject
{
    Q_OBJECT

public:
    cPCBInterface();
    virtual void setClient(Zera::Proxy::cProxyClient *client);
    // all commands to sense interface
    virtual quint32 getDSPChannel(QString chnName); // int
    virtual quint32 getStatus(QString chnName); // int
    virtual quint32 resetStatus(QString chnName); // reply (ack, nak..)
    virtual quint32 getAlias(QString chnName); // qstring
    virtual quint32 getType(QString chnName); // qstring
    virtual quint32 getUnit(QString chnName); // qstring
    virtual quint32 getRange(QString chnName); // qstring
    virtual quint32 getRangeList(QString chnName); // qstringlist
    virtual quint32 getAlias(QString chnName, QString rngName); // qstring
    virtual quint32 getType(QString chnName, QString rngName); // int
    virtual quint32 getUrvalue(QString chnName, QString rngName); // double
    virtual quint32 getUrvalue(QString chnName); // double
    virtual quint32 getRejection(QString chnName, QString rngName); // double
    virtual quint32 getOVRejection(QString chnName, QString rngName); // double
    virtual quint32 isAvail(QString chnName, QString rngName); // bool
    virtual quint32 getGainCorrection(QString chnName, QString rngName, double ampl); // double
    virtual quint32 getOffsetCorrection(QString chnName, QString rngName, double ampl); // double
    virtual quint32 getPhaseCorrection(QString chnName, QString rngName, double ampl); // double
    virtual quint32 setRange(QString chnName, QString rngName); // reply (ack, nak..)
    virtual quint32 getSampleRate(); // int the actual set sampling rate

    // all commands to source interface
    virtual quint32 getAliasSource(QString chnName); // qstring
    virtual quint32 getDSPChannelSource(QString chnName); // int
    virtual quint32 getFormFactorSource(QString chnName); // double

    virtual quint32 registerNotifier(QString query, QString notifier); // register for notification on change
    virtual quint32 unregisterNotifiers(); // unregister from all notifications


signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    Q_DECLARE_PRIVATE(cPCBInterface)
    cPCBInterfacePrivate *d_ptr;
};

}
}

#endif // PCBINTERFACE_H
