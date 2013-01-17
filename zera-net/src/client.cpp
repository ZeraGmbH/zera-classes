#include "client.h"

namespace ZeraNet
{
  Client::Client(int socketDescriptor, QObject *parent) :
    QObject(parent)
  {
    sockDescriptor=socketDescriptor;

    if(!clSocket.setSocketDescriptor(sockDescriptor))
    {
      emit error(clSocket.error());
      qDebug()<<"error setting clients socket descriptor";
      return;
    }

    connect(&timeoutCheck, SIGNAL(timeout()), this,SIGNAL(timeout()));

    connect(&clSocket,SIGNAL(readyRead()),this,SIGNAL(clientReceiveNotification()));
  }


  QString Client::readClient()
  {
    if(clSocket.waitForReadyRead(150))//wait for 150 mseconds max
    {
      refresh(); //the connection is alive
      QString retVal;
      QDataStream in(&clSocket);
      in.setVersion(QDataStream::Qt_4_0);
      quint16 tmp;
      in >> tmp;
      if(clSocket.bytesAvailable()<tmp)
      {
        qDebug()<<"Error bytes not available";
        retVal="";
      }
      else
      {
        in >> retVal;
      }
      return retVal;
    }
    else
      return "";
  }

  void Client::writeClient(QString message)
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    clSocket.write(block);
    if(clSocket.waitForBytesWritten(150)) // wait 150 mseconds max
      refresh(); //the connection is alive

  }

  int Client::getSocket()
  {
    return sockDescriptor;
  }


  void Client::refresh()
  {
    timeoutCheck.start(150);
  }

  void Client::sendToClient(QString message)
  {
    writeClient(message);
  }
}
