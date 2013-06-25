#include "clientnetbase_private.h"

#include <QTcpSocket>
#include <google/protobuf/message.h>

#include <QDebug>

namespace Zera
{
  namespace NetClient
  {
    _ClientNetBasePrivate::_ClientNetBasePrivate(QObject *parent)
      : QObject(parent)
    {
    }

    bool _ClientNetBasePrivate::readMessage(google::protobuf::Message *message, const QByteArray &array)
    {
      return message->ParseFromArray(array, array.size());
    }

    void _ClientNetBasePrivate::sendMessage(google::protobuf::Message *message)
    {
      sendByteArray(QByteArray(message->SerializeAsString().c_str(), message->ByteSize()));
    }


    void _ClientNetBasePrivate::startNetwork(QString ipAddress, quint16 port)
    {
      tcpSock= new QTcpSocket(this);
      tcpSock->connectToHost(ipAddress, port);
      if(tcpSock->isWritable())
      {
        connect(tcpSock, SIGNAL(readyRead()), this, SLOT(newMessage()));
        connect(tcpSock, SIGNAL(disconnected()), this, SIGNAL(connectionLost()));
      }
      tcpSock->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    }

    void _ClientNetBasePrivate::newMessage()
    {
      QTcpSocket *client = reinterpret_cast<QTcpSocket *> (QObject::sender());
      QDataStream in(client);
      in.setVersion(QDataStream::Qt_4_0);
      quint16 expectedSize;
      in >> expectedSize;
      if(client->bytesAvailable()<expectedSize)
      {
        qDebug()<<"Error bytes not available";
      }
      else
      {
        QByteArray message;
        in >> message;
        messageAvailable(message);
      }
    }

    void _ClientNetBasePrivate::sendByteArray(const QByteArray &bA)
    {
      QByteArray block;
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_4_0);
      out << (quint16)0;
      //qDebug()<<"Sending message:"<<QString(bA.toBase64());
      out << bA;
      out.device()->seek(0);
      out << (quint16)(block.size() - sizeof(quint16));

      if(!tcpSock->write(block))
      {
        qDebug()<<"Failed to send message";
      }
    }
  }
}



