#include "client_pub.h"
#include "client.h"

namespace Zera
{
  namespace Net
  {

    Client::Client(int socketDescriptor, QObject *parent) : QObject(parent)
    {
      d_ptr=new Zera::Net::_ClientPrivate(socketDescriptor);
      connect(d_ptr,SIGNAL(error(QTcpSocket::SocketError)),this,SIGNAL(error(QTcpSocket::SocketError)));
      connect(d_ptr,SIGNAL(readyRead()),this,SIGNAL(readyRead()));
      connect(d_ptr,SIGNAL(timeout()),this, SIGNAL(timeout()));
    }

    int Client::getSocket()
    {
      return d_ptr->getSocket();
    }

    QString Client::readClient()
    {
      return d_ptr->readClient();
    }

    void Client::sendToClient(QString message)
    {
      d_ptr->sendToClient(message);
    }

    void Client::refresh()
    {
      d_ptr->refresh();
    }
  }
}
