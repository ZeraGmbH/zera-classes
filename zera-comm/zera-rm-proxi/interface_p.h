#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <QObject>
#include <QString>
#include <netmessages.pb.h>

#include "rmproxi_global.h"
#include "interface.h"

namespace Zera
{
namespace RMProxi
{


class cInterfacePrivate: public cInterface
{

   Q_OBJECT

public:
    cInterfacePrivate(QObject* parent = 0);
    virtual void addResource(QString type, QString name, int n, QString description, quint16 port);
    virtual void removeResource(QString type, QString name);
    virtual void getResourceTypes();
    virtual void getResources(QString type);
    virtual void getResourceInfo(QString type, QString name);
    virtual void setResource(QString type, QString name, int n);
    virtual void freeResource(QString type, QString name);

    virtual void transferAnswer(ProtobufMessage::NetMessage *message);

signals:
    void rmCommand(ProtobufMessage::NetMessage *message);

private:
    void sendCommand(QString cmd);
    void sendCommand(QString cmd, QString par);
};


}
}
#endif // INTERFACE_P_H
