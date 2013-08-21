#include "zeraclientnetbase.h"

#include "zeraclientnetbaseprivate.h"
#include <QTcpSocket>
#include <google/protobuf/message.h>

namespace Zera
{
  namespace NetClient
  {
    cClientNetBase::cClientNetBase(QObject *parent) :
      QObject(parent), d_ptr(new cClientNetBasePrivate(this))
    {

    }

    bool cClientNetBase::readMessage(google::protobuf::Message *message, const QByteArray &array)
    {
      return message->ParseFromArray(array, array.size());
    }

    void cClientNetBase::sendMessage(google::protobuf::Message *message)
    {
      Q_D(cClientNetBase);
      d->sendByteArray(QByteArray(message->SerializeAsString().c_str(), message->ByteSize()));
    }

    void cClientNetBase::startNetwork(QString ipAddress, quint16 port)
    {
      Q_D(cClientNetBase);

      d->tcpSock= new QTcpSocket(this);
      connect(d->tcpSock, SIGNAL(connected()), this, SIGNAL(connected()));
      connect(d->tcpSock, SIGNAL(readyRead()), this, SLOT(newMessage()));
      connect(d->tcpSock, SIGNAL(disconnected()), this, SIGNAL(connectionLost()));
      connect(d->tcpSock, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(tcpError(QAbstractSocket::SocketError)));
      d->tcpSock->connectToHost(ipAddress, port);
      d->tcpSock->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    }


    void cClientNetBase::disconnectFromServer()
    {
      Q_D(cClientNetBase);
      d->tcpSock->close();
    }

    void cClientNetBase::newMessage()
    {
      Q_D(cClientNetBase);
      QByteArray tmpMessage;
      tmpMessage=d->readClient();
      while(!tmpMessage.isNull())
      {
        emit messageAvailable(tmpMessage);
        tmpMessage=d->readClient();
      }
    }

  }
}
