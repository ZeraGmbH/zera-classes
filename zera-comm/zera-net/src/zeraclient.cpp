#include "zeraclient.h"
#include "zclient_private.h"

namespace Zera
{
  namespace Net
  {

    ZeraClient::ZeraClient(quint32 socketDescriptor, QString name, QObject *parent) : QObject(parent)
    {
      // will be autodeleted if the socket is disconnected
      d_ptr=new Zera::Net::_ZClientPrivate(socketDescriptor, name, this);
      connect(d_ptr,SIGNAL(sockError(QAbstractSocket::SocketError)),this,SIGNAL(error(QAbstractSocket::SocketError)));
      connect(d_ptr,SIGNAL(messageReceived(QByteArray)),this,SIGNAL(messageReceived(QByteArray)));
      connect(d_ptr,SIGNAL(clientDisconnected()),this,SIGNAL(clientDisconnected()));
    }

    QHostAddress ZeraClient::getIpAddress()
    {
      return d_ptr->getIpAddress();
    }

    const QString &ZeraClient::getName()
    {
      return d_ptr->getName();
    }

    quint32 ZeraClient::getSocket()
    {
      return d_ptr->getSocket();
    }

    bool ZeraClient::translateBA2Protobuf(google::protobuf::Message *message, const QByteArray &array)
    {
      return d_ptr->translateBA2Protobuf(message, array);
    }

    QByteArray ZeraClient::translatePB2ByteArray(google::protobuf::Message *message)
    {
      return d_ptr->translatePB2ByteArray(message);
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
