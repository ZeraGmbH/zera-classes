#include "zclient_private.h"


#include <QState>
#include <QFinalState>
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    _ZClientPrivate::_ZClientPrivate(quint32 socketDescriptor, QString clientName, QObject *parent) :
      QStateMachine(parent),
      name(clientName),
      sockDescriptor(socketDescriptor)
    {
      clSocket = new QTcpSocket(this);

      if(!clSocket->setSocketDescriptor(sockDescriptor))
      {
        emit sockError(clSocket->error());
        qFatal("[zera-net]error setting clients socket descriptor");
        return;
      }
      clSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
      connect(clSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SIGNAL(sockError(QAbstractSocket::SocketError)));
      connect(clSocket,SIGNAL(disconnected()),this,SLOT(disconnectClient()));

      setupStateMachine();
    }

    QHostAddress _ZClientPrivate::getIpAddress()
    {
      return clSocket->peerAddress();
    }

    const QString &_ZClientPrivate::getName()
    {
      return name;
    }

    quint32 _ZClientPrivate::getSocket()
    {
      return sockDescriptor;
    }

    QByteArray _ZClientPrivate::readClient()
    {
      if(clSocket->bytesAvailable())
      {
        QByteArray retVal;
        QDataStream in(clSocket);
        in.setVersion(QDataStream::Qt_4_0);
        quint16 expectedSize;
        in >> expectedSize;
        if(clSocket->bytesAvailable()<expectedSize)
        {
          //error
          qWarning("[zera-net]Error bytes not available");
        }
        else
        {
          in >> retVal;
          //qDebug()<<"[zera-net]Receiving message:"<<QString(retVal.toBase64());
        }
        return retVal;
      }
      else
        return QByteArray();
    }

    bool _ZClientPrivate::translateBA2Protobuf(google::protobuf::Message *message, const QByteArray &array)
    {
      return message->ParseFromArray(array, array.size());
    }

    QByteArray _ZClientPrivate::translatePB2ByteArray(const google::protobuf::Message &message)
    {
      return QByteArray(message.SerializeAsString().c_str(), message.ByteSize());
    }

    void _ZClientPrivate::logoutClient()
    {
      emit clientLogout();
      //placeholder
    }


    void _ZClientPrivate::setName(QString newName)
    {
      name=newName;
    }

    void _ZClientPrivate::writeClient(QByteArray message)
    {
      //qDebug()<<"[zera-net]Sending message:"<<QString(message.toBase64());
      QByteArray block;
      // serialize the ProtobufMessage to send it
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_4_0);
      out << (quint16)0;
      out << message;
      out.device()->seek(0);
      out << (quint16)(block.size() - sizeof(quint16));

      clSocket->write(block);
    }

    void _ZClientPrivate::initialize()
    {
      emit clientConnected();
      connect(clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
    }

    void _ZClientPrivate::maintainConnection()
    {
      QByteArray newMessage;
      newMessage=readClient();
      while(!newMessage.isNull())
      {
        emit messageReceived(newMessage);
        newMessage=readClient();
      }
    }

    void _ZClientPrivate::disconnectClient()
    {
      // as we will disconnect, do not read from the socket anymore
      disconnect(clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
      if(clSocket!=0 && clSocket->isOpen())
      {
        clSocket->disconnectFromHost();
        clSocket->close();
      }
      qDebug()<<"[zera-net]Client disconnected: "<< name;
      emit clientDisconnected();
    }

    void _ZClientPrivate::setupStateMachine()
    {
      stContainer = new QState(this);
      stInit = new QState(stContainer);
      stConnected = new QState(stContainer);
      stAboutToDisconnect = new QState(stContainer);
      fstDisconnected = new QFinalState(this);

      this->setInitialState(stContainer);

      stContainer->setInitialState(stInit);
      stContainer->addTransition(clSocket,SIGNAL(disconnected()),fstDisconnected);
      stContainer->addTransition(this, SIGNAL(clientLogout()),stAboutToDisconnect);

      stInit->addTransition(this, SIGNAL(clientConnected()), stConnected);

      connect(stInit,SIGNAL(entered()),this,SLOT(initialize()));
      //obsolete
      //connect(stConnected,SIGNAL(entered()),this,SLOT(maintainConnection()));
      connect(stAboutToDisconnect,SIGNAL(entered()),this,SLOT(disconnectClient()));
    }
  }
}
