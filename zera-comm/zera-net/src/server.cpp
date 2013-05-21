#include "server.h"
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    QList<Zera::Net::ZeraClient *> _ServerPrivate::getClients()
    {
      return clients;
    }

    _ServerPrivate* _ServerPrivate::getInstance()
    {
      if(singletonInstance==0)
      {
        singletonInstance=new _ServerPrivate;
      }
      return singletonInstance;
    }

    void _ServerPrivate::broadcastMessage(QByteArray message)
    {
      foreach(ZeraClient* c,clients)
      {
        c->writeClient(message);
      }
    }

    void _ServerPrivate::clientDisconnectedSRV()
    {
      if(QObject::sender()!=0)
      {
        ZeraClient* client =  reinterpret_cast<ZeraClient*>(QObject::sender());
        clients.removeAll(client);
        client->deleteLater();
      }
    }

    void _ServerPrivate::startServer(quint16 port)
    {

      /// @todo change default port
      this->listen(QHostAddress::Any, port);
      qDebug()<<"[zera-net]Server Started";
    }

    _ServerPrivate::_ServerPrivate(QObject* parent) :
      QTcpServer(parent)
    {
    }

    _ServerPrivate::~_ServerPrivate()
    {
      foreach(ZeraClient *c, clients)
      {
        c->deleteLater();
      }
    }

    void _ServerPrivate::incomingConnection(int socketDescriptor)
    {
      //qDebug()<<"[zera-net]Client connected";

      ZeraClient *client = new ZeraClient(socketDescriptor);
      clients.append(client);
      connect(client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnectedSRV()));
      emit newClientAvailable(client);
      client->start();
    }

    _ServerPrivate* _ServerPrivate::singletonInstance=0;
  }
}
