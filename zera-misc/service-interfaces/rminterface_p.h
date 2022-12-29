#ifndef RMINTERFACE_P_H
#define RMINTERFACE_P_H

#include <QObject>
#include <QStringList>

#include "interface_p.h"
#include "rminterface.h"
#include "dspmeasdata.h"

namespace Zera
{
namespace Server
{

enum rmcommands
{
    rmident,
    addresource,
    removeresource,
    getresourcetypes,
    getresources,
    getresourceinfo,
    setresource,
    freeresource
};


class cRMInterface;

class cRMInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT

public:
    cRMInterfacePrivate(cRMInterface* iface);
    void setClient(Zera::Proxy::cProxyClient *client);
    void setClientSmart(Zera::Proxy::ProxyClientPtr client);
    virtual quint32 rmIdent(QString name);
    virtual quint32 addResource(QString type, QString name, int n, QString description, quint16 port);
    virtual quint32 removeResource(QString type, QString name);
    virtual quint32 getResourceTypes();
    virtual quint32 getResources(QString type);
    virtual quint32 getResourceInfo(QString type, QString name);
    virtual quint32 setResource(QString type, QString name, int n);
    virtual quint32 freeResource(QString type, QString name);

protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cRMInterface)
    cRMInterface *q_ptr;
    Zera::Proxy::ProxyClientPtr m_clientSmart;

    QStringList CycCmdList, IntCmdList;
    QList<cDspMeasData*> m_DspMeasDataList; // eine liste mit zeigern auf "programmdaten"
    QList<cDspMeasData*> m_DspMemoryDataList; // eine liste mit zeigern auf  dsp speicher allgemein

};

}
}


#endif // RMINTERFACE_P_H
