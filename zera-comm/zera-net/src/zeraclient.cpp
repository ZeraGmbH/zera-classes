#include "zeraclient.h"
#include "client.h"

namespace Zera
{
  namespace Net
  {

    ZeraClient::ZeraClient(quint32 socketDescriptor, QString name, QObject *parent) : QObject(parent)
    {
      // will be autodeleted if the socket is disconnected
      d_ptr=new Zera::Net::_ClientPrivate(socketDescriptor, name, this);
      connect(d_ptr,SIGNAL(sockError(QAbstractSocket::SocketError)),this,SIGNAL(error(QAbstractSocket::SocketError)));
      connect(d_ptr,SIGNAL(messageReceived(QByteArray)),this,SIGNAL(messageReceived(QByteArray)));
      connect(d_ptr,SIGNAL(clientDisconnected()),this,SIGNAL(clientDisconnected()));
    }

    const QString &ZeraClient::getName()
    {
      return d_ptr->getName();
    }

    int ZeraClient::getSocket()
    {
      return d_ptr->getSocket();
    }

    void ZeraClient::logoutClient()
    {
      d_ptr->logoutClient();
    }

    void ZeraClient::setName(QString newName)
    {
      d_ptr->setName(newName);
    }

    void ZeraClient::start()
    {
      d_ptr->start();
    }

    void ZeraClient::writeClient(QByteArray message)
    {
      d_ptr->writeClient(message);
    }

    QByteArray ZeraClient::readClient()
    {
      return d_ptr->readClient();
    }
  }
}
