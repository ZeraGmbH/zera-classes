#ifndef PCBPROXY_P_H
#define PCBPROXY_P_H

#include <QObject>
#include <QHash>
#include <QString>
#include <zeraclientnetbase.h>

#include "pcbproxy.h"
#include "interface_p.h"

namespace Zera
{
namespace PCBProxy
{

class cPCBProxy;

class cPCBProxyPrivate: public QObject
{
    Q_OBJECT

protected:
    cPCBProxyPrivate(cPCBProxy *parent);
    ~cPCBProxyPrivate(){}

    cInterface* getInterface(quint16 port); // proxy has to generate network connections dynamically
    void setIPAdress(QString ipAddress);

    /**
       * @brief singletonInstance
       */
    static cPCBProxy* singletonInstance;

    /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
    cPCBProxy *q_ptr;

protected slots:
    void transferCommand(ProtobufMessage::NetMessage *message);
    void receiveMessage(QByteArray message);

private:
    Q_DISABLE_COPY(cPCBProxyPrivate)
    Q_DECLARE_PUBLIC(cPCBProxy)

    QString m_sIPAdress; // ip adress for all pcbservers, default localhost

    // holds network connection for each port
    QHash<quint16, Zera::NetClient::cClientNetBase*> m_PCBServerHash;
    // information about the interfaces network connection
    QHash<Zera::PCBProxy::cInterfacePrivate*, Zera::NetClient::cClientNetBase*> m_InterfaceHash;
    // information for redirecting
    QHash<QByteArray, Zera::PCBProxy::cInterfacePrivate*> m_UUIDHash;

};

}
}


#endif // PCBPROXY_P_H
