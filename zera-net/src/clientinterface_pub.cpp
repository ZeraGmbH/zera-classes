#include "clientinterface_pub.h"

namespace Zera
{
  namespace Net
  {



    ClientInterface::ClientInterface(QObject *parent) : QObject(parent)
    {
      d_ptr = Zera::Net::_ClientInterfacePrivate::getInstance();
    }

    ClientInterface::~ClientInterface()
    {
      d_ptr->deleteLater();
    }

    ClientInterface *ClientInterface::getInstance()
    {
      if(singletonInstance==0)
      {
        singletonInstance=new ClientInterface;
      }
      return singletonInstance;
    }

    void ClientInterface::clientDisconnect()
    {
      d_ptr->clientDisconnect();
    }

  }
}
