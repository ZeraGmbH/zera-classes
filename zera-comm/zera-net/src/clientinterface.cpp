#include "clientinterface.h"
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    _ClientInterfacePrivate::_ClientInterfacePrivate(QObject* parent) :
      QTcpServer(parent)
    {

      /// @todo change default port
      this->listen(QHostAddress::Any, 12345);
      qDebug()<<"Server Started";
    }

    _ClientInterfacePrivate::~_ClientInterfacePrivate()
    {
      foreach(_ClientPrivate *c, clients)
      {
        c->deleteLater();
      }
    }

    void _ClientInterfacePrivate::incomingConnection(int socketDescriptor)
    {
      qDebug()<<"Client connected";

      _ClientPrivate *client = new _ClientPrivate(socketDescriptor);
      clients.append(client);
      //connect(client, SIGNAL(timeout()), this, SLOT(clientDisconnect()));

    }

    void _ClientInterfacePrivate::clientDisconnect()
    {
      if(QObject::sender()!=0)
      {
        _ClientPrivate* client =  reinterpret_cast<_ClientPrivate*>(QObject::sender());
        clients.removeAll(client);
        client->deleteLater();
      }
    }

    _ClientInterfacePrivate* _ClientInterfacePrivate::singletonInstance=0;

    _ClientInterfacePrivate* _ClientInterfacePrivate::getInstance()
    {
      if(singletonInstance==0)
      {
        singletonInstance=new _ClientInterfacePrivate;
      }
      return singletonInstance;
    }
  }
}
