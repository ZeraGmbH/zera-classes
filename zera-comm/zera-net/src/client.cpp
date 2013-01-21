#include "client.h"

namespace Zera
{
  namespace Net
  {
    _ClientPrivate::_ClientPrivate(int socketDescriptor, QObject *parent) :
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

      connect(&clSocket,SIGNAL(readyRead()),this,SIGNAL(readyRead()));
    }


    QString _ClientPrivate::readClient()
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

    void _ClientPrivate::writeClient(QString message)
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

    int _ClientPrivate::getSocket()
    {
      return sockDescriptor;
    }


    void _ClientPrivate::refresh()
    {
      timeoutCheck.start(150);
    }

    void _ClientPrivate::sendToClient(QString message)
    {
      writeClient(message);
    }
  }
}
