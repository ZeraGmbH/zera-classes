#ifndef SECINTERFACE_P_H
#define SECINTERFACE_P_H


#include <QObject>
#include <QStringList>

#include "interface_p.h"
#include "secinterface.h"

namespace Zera
{
namespace Server
{

enum seccommands
{
    setecalcunit,
    freeecalcunit,
    writeregister,
    readregister,
    setsync,
    setmux,
    setcmdid,
    startecalc,
    stopecalc,
    regnotifier,
    unregnotifier
};


class cSECInterface;

class cSECInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT

public:
    cSECInterfacePrivate(cSECInterface* iface);
    virtual void setClient(Zera::Proxy::cProxyClient *client);
    virtual quint32 setECalcUnit(int n); // we want n eclac units, answer is the names of them
    virtual quint32 freeECalcUnits(); // free all the units the client had set
    virtual quint32 writeRegister(QString chnname, quint8 reg, quint32 value); // transp. register access
    virtual quint32 readRegister(QString chnname, quint8 reg);

    virtual quint32 setSync(QString chnname, QString syncChn);
    virtual quint32 setMux(QString chnname, quint8 mux);
    virtual quint32 setCmdid(QString chnname, quint8 cmdid);
    virtual quint32 start(QString chnname);
    virtual quint32 stop(QString chnname);

    virtual quint32 registerNotifier(QString query, QString notifier); // register for notification on change
    virtual quint32 unregisterNotifiers(); // unregister from all notifications

protected slots:
    virtual void receiveAnswer(ProtobufMessage::NetMessage *message);
    virtual void receiveError(QAbstractSocket::SocketError errorCode);

private:
    Q_DECLARE_PUBLIC(cSECInterface)
    cSECInterface *q_ptr;
};

}
}



#endif // SECINTERFACE_P_H
