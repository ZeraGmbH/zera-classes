#include "zserver_private.h"
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    QList<Zera::Net::ZeraClient *> _ZServerPrivate::getClients()
    {
      return clients;
    }

    _ZServerPrivate* _ZServerPrivate::getInstance()
    {
      if(singletonInstance==0)
      {
        singletonInstance=new _ZServerPrivate;
      }
      return singletonInstance;
    }

    void _ZServerPrivate::broadcastMessage(QByteArray message)
    {
      foreach(ZeraClient* c,clients)
      {
        c->writeClient(message);
      }
    }

    void _ZServerPrivate::clientDisconnectedSRV()
    {
      if(QObject::sender()!=0)
      {
        ZeraClient* client =  reinterpret_cast<ZeraClient*>(QObject::sender());
        clients.removeAll(client);
        client->deleteLater();
      }
    }

    void _ZServerPrivate::startServer(quint16 port)
    {

      /// @todo change default port
      this->listen(QHostAddress::Any, port);
      qDebug()<<"[zera-net]Server Started";
    }

    _ZServerPrivate::_ZServerPrivate(QObject* parent) :
      QTcpServer(parent)
    {
    }

    _ZServerPrivate::~_ZServerPrivate()
    {
      foreach(ZeraClient *c, clients)
      {
        c->deleteLater();
      }
    }

    void _ZServerPrivate::incomingConnection(int socketDescriptor)
    {
      //qDebug()<<"[zera-net]Client connected";

      ZeraClient *client = new ZeraClient(socketDescriptor);
      clients.append(client);
      connect(client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnectedSRV()));
      emit newClientAvailable(client);
      client->start();
    }

    _ZServerPrivate* _ZServerPrivate::singletonInstance=0;
  }
}
