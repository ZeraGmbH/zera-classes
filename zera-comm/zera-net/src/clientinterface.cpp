#include "clientinterface.h"
#include <QDebug>

namespace ZeraNet
{
  ClientInterface::ClientInterface(QObject* parent) :
    QTcpServer(parent)
  {

    /// @todo change default port
    this->listen(QHostAddress::Any, 12345);
    qDebug()<<"Server Started";
  }

  ClientInterface::~ClientInterface()
  {
    foreach(Client *c, clients)
    {
      c->deleteLater();
    }
  }

  void ClientInterface::incomingConnection(int socketDescriptor)
  {
    qDebug()<<"Client connected";

    Client *client = new Client(socketDescriptor);
    clients.append(client);
    //connect(client, SIGNAL(timeout()), this, SLOT(clientDisconnect()));

  }

  void ClientInterface::clientDisconnect()
  {
    if(QObject::sender()!=0)
    {
      Client* client = (Client*) QObject::sender();
      qDebug()<<"Client disconnected:"<<client->getName();
      clients.removeAll(client);
      client->deleteLater();
    }
  }

  ClientInterface* ClientInterface::singletonInstance=0;

  ClientInterface* ClientInterface::getInstance()
  {
    if(singletonInstance==0)
    {
      singletonInstance=new ClientInterface;
    }
    return singletonInstance;
  }
}
