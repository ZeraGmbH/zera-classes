#include "zeraclient.h"
#include "zeraclientprivate.h"
#include <QFinalState>

namespace Zera
{
  namespace Net
  {

    cClient::cClient(quint32 socketDescriptor, QString clientName, QObject *parent) :
      QStateMachine(parent),
      d_ptr(new Zera::Net::cClientPrivate(socketDescriptor))
    {
      Q_D(cClient);
      d->clSocket = new QTcpSocket(this);
      d->name =  clientName;

      if(!d->clSocket->setSocketDescriptor(socketDescriptor))
      {
        emit sockError(d->clSocket->error());
        qFatal("[zera-net]error setting clients socket descriptor");
        return;
      }
      d->clSocket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
      connect(d->clSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(sockError(QAbstractSocket::SocketError)));
      connect(d->clSocket, SIGNAL(disconnected()), this, SLOT(disconnectClient()));

      setupStateMachine();
    }

    cClient::~cClient()
    {
      delete d_ptr;
    }

    QHostAddress cClient::getIpAddress()
    {
      Q_D(cClient);
      return d->clSocket->peerAddress();
    }

    const QString &cClient::getName()
    {
      Q_D(cClient);
      return d->name;
    }

    quint32 cClient::getSocket()
    {
      Q_D(cClient);
      return d->sockDescriptor;
    }

    QByteArray cClient::readClient()
    {
      Q_D(cClient);
      if(d->clSocket->bytesAvailable())
      {
        QByteArray retVal;
        QDataStream in(d->clSocket);
        in.setVersion(QDataStream::Qt_4_0);
        quint16 expectedSize;
        in >> expectedSize;
        if(d->clSocket->bytesAvailable()<expectedSize)
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

    bool cClient::translateBA2Protobuf(google::protobuf::Message *message, const QByteArray &array)
    {
      return message->ParseFromArray(array, array.size());
    }

    QByteArray cClient::translatePB2ByteArray(google::protobuf::Message *message)
    {
      return QByteArray(message->SerializeAsString().c_str(), message->ByteSize());
    }

    void cClient::logoutClient()
    {
      emit clientLogout();
      //placeholder
    }


    void cClient::setName(QString newName)
    {
      Q_D(cClient);
      d->name=newName;
    }

    void cClient::writeClient(QByteArray message)
    {
      Q_D(cClient);
      //qDebug()<<"[zera-net]Sending message:"<<QString(message.toBase64());
      QByteArray block;
      // serialize the ProtobufMessage to send it
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_4_0);
      out << (quint16)0;
      out << message;
      out.device()->seek(0);
      out << (quint16)(block.size() - sizeof(quint16));

      d->clSocket->write(block);
    }

    void cClient::initialize()
    {
      Q_D(cClient);
      emit clientConnected();
      connect(d->clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
    }

    void cClient::maintainConnection()
    {
      QByteArray newMessage;
      newMessage=readClient();
      while(!newMessage.isNull())
      {
        emit messageReceived(newMessage);
        newMessage=readClient();
      }
    }

    void cClient::disconnectClient()
    {
      Q_D(cClient);
      // as we will disconnect, do not read from the socket anymore
      disconnect(d->clSocket,SIGNAL(readyRead()),this,SLOT(maintainConnection()));
      if(d->clSocket!=0 && d->clSocket->isOpen())
      {
        d->clSocket->disconnectFromHost();
        d->clSocket->close();
      }
      qDebug()<<"[zera-net]Client disconnected: "<< d->name;
      emit clientDisconnected();
    }

    void cClient::setupStateMachine()
    {
      Q_D(cClient);
      d->stContainer = new QState(this);
      d->stInit = new QState(d->stContainer);
      d->stConnected = new QState(d->stContainer);
      d->stAboutToDisconnect = new QState(d->stContainer);
      d->fstDisconnected = new QFinalState(this);

      this->setInitialState(d->stContainer);

      d->stContainer->setInitialState(d->stInit);
      d->stContainer->addTransition(d->clSocket, SIGNAL(disconnected()), d->fstDisconnected);
      d->stContainer->addTransition(this, SIGNAL(clientLogout()), d->stAboutToDisconnect);

      d->stInit->addTransition(this, SIGNAL(clientConnected()), d->stConnected);

      connect(d->stInit, SIGNAL(entered()), this,SLOT(initialize()));
      //obsolete
      //connect(stConnected,SIGNAL(entered()),this,SLOT(maintainConnection()));
      connect(d->stAboutToDisconnect, SIGNAL(entered()), this, SLOT(disconnectClient()));
    }
  }
}
