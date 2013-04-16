#include "zeraserver.h"

namespace Zera
{
  namespace Net
  {
    QList<Zera::Net::ZeraClient *> ZeraServer::getClients()
    {
      return d_ptr->getClients();
    }

    ZeraServer *ZeraServer::getInstance()
    {
      if(singletonInstance==0)
      {
        singletonInstance=new ZeraServer;
      }
      return singletonInstance;
    }

    void ZeraServer::broadcastMessage(QByteArray message)
    {
      d_ptr->broadcastMessage(message);
    }

    void ZeraServer::startServer(quint16 port)
    {
      d_ptr->startServer(port);
    }

    ZeraServer::ZeraServer(QObject *parent) : QObject(parent)
    {
      d_ptr = Zera::Net::_ServerPrivate::getInstance();
      connect(d_ptr,SIGNAL(newClientAvailable(Zera::Net::ZeraClient*)), this, SIGNAL(newClientAvailable(Zera::Net::ZeraClient*)));
    }

    ZeraServer::~ZeraServer()
    {
      d_ptr->deleteLater();
    }

    ZeraServer* ZeraServer::singletonInstance=0;
  }
}
