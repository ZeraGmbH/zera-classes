#ifndef RMPROXI_P_H
#define RMPROXI_P_H

#include <QObject>
#include <QHash>
#include <QByteArray>
#include <netmessages.pb.h>
#include <zeraclientnetbase.h>


#include "rmproxi_global.h"


const QString proxiName = "rmProxi";


namespace google {
  namespace protobuf {
    class Message;
  }
}

class QStateMachine;
class QState;

namespace Zera
{
namespace RMProxi
{

class cRMProxi;
class cInterface;
class cInterfacePrivate;


class cRMProxiPrivate : public Zera::NetClient::cClientNetBase
{

protected:
    cRMProxiPrivate(cRMProxi *parent);
    ~cRMProxiPrivate(){}

    cInterface* getInterface();

    /**
       * @brief singletonInstance
       */
    static cRMProxi* singletonInstance;

    /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
    cRMProxi *q_ptr;

protected slots:
    void transferCommand(ProtobufMessage::NetMessage *message);
    void receiveMessage(QByteArray message);

private:
    Q_DISABLE_COPY(cRMProxiPrivate)
    Q_DECLARE_PUBLIC(cRMProxi)

    QHash<QByteArray, Zera::RMProxi::cInterfacePrivate*> m_UUIDHash;
    QStateMachine* m_pStateMachine;
    QState* m_pStateConnect;
    QState* m_pStateIdent;

private slots:
    void entryIdent();

};


}
}
#endif // RMPROXI_P_H
