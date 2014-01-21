#ifndef RMPROXY_P_H
#define RMPROXY_P_H

#include <QObject>
#include <QHash>
#include <QByteArray>
#include <netmessages.pb.h>
#include <zeraclientnetbase.h>


#include "rmproxy_global.h"


const QString proxyName = "rmProxy";


namespace google {
  namespace protobuf {
    class Message;
  }
}

class QStateMachine;
class QState;

namespace Zera
{
namespace RMProxy
{

class cRMProxy;
class cInterface;
class cInterfacePrivate;


class cRMProxyPrivate : public Zera::NetClient::cClientNetBase
{

protected:
    cRMProxyPrivate(cRMProxy *parent);
    ~cRMProxyPrivate(){}

    cInterface* getInterface();

    /**
       * @brief singletonInstance
       */
    static cRMProxy* singletonInstance;

    /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
    cRMProxy *q_ptr;

protected slots:
    void transferCommand(ProtobufMessage::NetMessage *message);
    void receiveMessage(QByteArray message);

private:
    Q_DISABLE_COPY(cRMProxyPrivate)
    Q_DECLARE_PUBLIC(cRMProxy)

    QHash<QByteArray, Zera::RMProxy::cInterfacePrivate*> m_UUIDHash;
    QStateMachine* m_pStateMachine;
    QState* m_pStateConnect;
    QState* m_pStateIdent;

private slots:
    void entryIdent();

};


}
}
#endif // RMPROXY_P_H
