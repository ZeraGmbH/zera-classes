#include "zeraserver.h"
#include "zeraserverprivate.h"

namespace Zera
{
  namespace Net
  {


    QList<Zera::Net::cClient *> cServer::getClients()
    {
      Q_D(cServer);
      return d->clients;
    }

    cServer *cServer::getInstance()
    {
      if(cServerPrivate::singletonInstance==0)
      {
        cServerPrivate::singletonInstance=new cServer;
      }
      return cServerPrivate::singletonInstance;
    }

    void cServer::broadcastMessage(QByteArray message)
    {
      Q_D(cServer);
      foreach(cClient* c,d->clients)
      {
        c->writeClient(message);
      }
    }

    void cServer::startServer(quint16 port)
    {
      /// @todo change default port
      this->listen(QHostAddress::Any, port);
      qDebug()<<"[zera-net]Server Started";
    }

    cServer::cServer(QObject *parent) : QTcpServer(parent), d_ptr(new Zera::Net::cServerPrivate(this))
    {
    }

    cServer::~cServer()
    {
      delete d_ptr;
    }

    void cServer::incomingConnection(int socketDescriptor)
    {
      Q_D(cServer);
      //qDebug()<<"[zera-net]Client connected";

      cClient *client = new cClient(socketDescriptor);
      d->clients.append(client);
      connect(client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnectedSRV()));
      emit newClientAvailable(client);
      client->start();

    }

    void cServer::clientDisconnectedSRV()
    {
      if(QObject::sender()!=0)
      {
        Q_D(cServer);
        cClient* client = qobject_cast<cClient*>(QObject::sender());
        if(client)
        {
          d->clients.removeAll(client);
          client->deleteLater();
        }
      }
    }

  }
}
