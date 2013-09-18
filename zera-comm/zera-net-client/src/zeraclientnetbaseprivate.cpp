#include "zeraclientnetbaseprivate.h"

#include "zeraclientnetbase.h"


#include <QDebug>

namespace Zera
{
  namespace NetClient
  {
    cClientNetBasePrivate::cClientNetBasePrivate(cClientNetBase *parent)
      : q_ptr(parent)
    {
    }

    QByteArray cClientNetBasePrivate::readClient()
    {
      if(tcpSock->bytesAvailable())
      {
        QByteArray retVal;
        QDataStream in(tcpSock);
        in.setVersion(QDataStream::Qt_4_0);
        qint64 expectedSize;
        in >> expectedSize;
        if(tcpSock->bytesAvailable()<expectedSize)
        {
          //error
          qWarning("[zera-net-client]Error bytes not available");
        }
        else
        {
          in >> retVal;
          //qDebug()<<"[zera-net-client]Receiving message:"<<QString(retVal.toBase64());
        }
        return retVal;
      }
      else
        return QByteArray();
    }

    void cClientNetBasePrivate::sendByteArray(const QByteArray &bA)
    {
      QByteArray block;
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_4_0);
      out << (qint64)0;
      //qDebug()<<"[zera-net-client] Sending message:"<<QString(bA.toBase64());
      out << bA;
      out.device()->seek(0);
      out << (qint64)(block.size() - sizeof(qint64));

      if(!tcpSock->write(block))
      {
        qDebug()<<"[zera-net-client] Failed to send message";
      }
    }
  }
}



