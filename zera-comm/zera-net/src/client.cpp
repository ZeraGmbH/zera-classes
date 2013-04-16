#include "client.h"

#include <QTcpSocket>
#include <QState>
#include <QFinalState>
#include <QTimer>
#include <QDebug>

namespace Zera
{
  namespace Net
  {
    _ClientPrivate::_ClientPrivate(quint32 socketDescriptor, QString clientName, QObject *parent) :
      QStateMachine(parent),
      name(clientName),
      sockDescriptor(socketDescriptor)
    {
      clSocket = new QTcpSocket(this);

      if(!clSocket->setSocketDescriptor(sockDescriptor))
      {
        emit error(clSocket->error());
        qFatal("error setting clients socket descriptor");
        return;
      }
      clSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
      connect(clSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SIGNAL(error(QAbstractSocket::SocketError)));
      connect(clSocket,SIGNAL(disconnected()),this,SLOT(disconnectClient()));

      setupStateMachine();
    }

    const QString &_ClientPrivate::getName()
    {
      return name;
    }

    int _ClientPrivate::getSocket()
    {
      return sockDescriptor;
    }

    QByteArray _ClientPrivate::readClient()
    {
      if(clSocket->bytesAvailable())  {
        QByteArray retVal;
        QDataStream in(clSocket);
        in.setVersion(QDataStream::Qt_4_0);
        quint16 expectedSize;
        in >> expectedSize;
        if(clSocket->bytesAvailable()<expectedSize)
        {
          //error
          qFatal("Error bytes not available");
        }
        else
        {
          in >> retVal;
          //qDebug()<<"Receiving message:"<<QString(retVal.toBase64());
        }
        return retVal;
      }
      else
        return QByteArray(); //ERROR
    }

    void _ClientPrivate::logoutClient()
    {
      emit clientLogout();
    }


    void _ClientPrivate::setName(QString newName)
    {
      name=newName;
    }

    void _ClientPrivate::writeClient(QByteArray message)
    {
      //qDebug()<<"Sending message:"<<QString(message.toBase64());
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

    void _ClientPrivate::initialize()
    {
      emit clientConnected();
      connect(clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
    }

    void _ClientPrivate::maintainConnection()
    {
      emit messageReceived(readClient());
    }

    void _ClientPrivate::disconnectClient()
    {
      // as we will disconnect, do not read from the socket anymore
      disconnect(clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
      if(clSocket!=0 && clSocket->isOpen())
      {
        clSocket->disconnectFromHost();
        clSocket->close();
      }
      qDebug()<<"Client disconnected";
      emit clientDisconnected();
    }

    void _ClientPrivate::setupStateMachine()
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
