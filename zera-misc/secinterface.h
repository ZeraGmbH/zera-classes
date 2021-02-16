#ifndef SECINTERFACE_H
#define SECINTERFACE_H

#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

#include "zera_misc_global.h"


namespace ECALCREG {
    enum { CMD, CONF, STATUS, INTMASK, INTREG, MTCNTin, MTCNTfin, MTCNTact, MTPULSin = 12, MTPAUSEin, MTPULS, MTPAUSE};
}

namespace ECALCCMDID {
    //enum { COUNTEDGE = 1, COUNTRESET, ERRORMEASMASTER, ERRORMEASSLAVE};
    enum {SINGLEERRORMASTER, CONTERRORMASTER, ERRORMEASSLAVE};
}

namespace ECALCSTATUS {
    enum { IDLE, ARMED, STARTED = 2, READY = 4, ABORT = 8};
}

namespace ECALCINT {
    enum { MTposEdge = 1, MTnegEdge = 2, MTCount0 = 4};
}

namespace ECALCRESULT {
    enum enResultTypes { RESULT_UNFINISHED = -1, RESULT_FAILED = 0, RESULT_PASSED = 1};
}


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

class cSECInterfacePrivate;


class ZERA_MISCSHARED_EXPORT cSECInterface: public QObject
{
    Q_OBJECT

public:
    cSECInterface();
    virtual void setClient(Zera::Proxy::cProxyClient *client);
    virtual quint32 setECalcUnit(int n); // we want n eclac units, answer is the names of them
    virtual quint32 freeECalcUnits(); // free all the units the client had set
    virtual quint32 writeRegister(QString chnname, quint8 reg, quint32 value); // transp. register access
    virtual quint32 readRegister(QString chnname, quint8 reg);

    virtual quint32 setSync(QString chnname, QString syncchn);
    virtual quint32 setMux(QString chnname, QString inpname);
    virtual quint32 setCmdid(QString chnname, quint8 cmdid);
    virtual quint32 start(QString chnname);
    virtual quint32 stop(QString chnname);
    virtual quint32 intAck(QString chnname, quint8 interrupt);

    virtual quint32 registerNotifier(QString query); // register for notification on change
    virtual quint32 unregisterNotifiers(); // unregister from all notifications
    static constexpr quint32 maxSecCounterInitVal = std::numeric_limits<quint32>::max() - 1;

signals:
    void tcpError(QAbstractSocket::SocketError errorCode);
    void serverAnswer(quint32 msgnr, quint8 reply, QVariant answer);

private:
    Q_DECLARE_PRIVATE(cSECInterface)
    cSECInterfacePrivate *d_ptr;
};

}
}


#endif // SECINTERFACE_H
