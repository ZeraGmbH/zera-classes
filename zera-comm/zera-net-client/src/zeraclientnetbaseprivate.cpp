#include "zeraclientnetbaseprivate.h"

#include "zeraclientnetbase.h"


#include <QDebug>

namespace Zera
{
  namespace NetClient
  {
    cClientNetBasePrivate::cClientNetBasePrivate()
    {
    }

    QByteArray cClientNetBasePrivate::readClient()
    {
      QDataStream in(tcpSock);
      QByteArray message = QByteArray();
      in.setVersion(QDataStream::Qt_4_0);
      quint16 expectedSize;
      in >> expectedSize;
      if(tcpSock->bytesAvailable()<expectedSize)
      {
        qDebug()<<"[zera-net-client] Error bytes not available";
      }
      else
      {
        Q_Q(cClientNetBase);
        in >> message;
        q->messageAvailable(message);
      }
      return message;
    }

    void cClientNetBasePrivate::sendByteArray(const QByteArray &bA)
    {
      QByteArray block;
      QDataStream out(&block, QIODevice::WriteOnly);
      out.setVersion(QDataStream::Qt_4_0);
      out << (quint16)0;
      //qDebug()<<"[zera-net-client] Sending message:"<<QString(bA.toBase64());
      out << bA;
      out.device()->seek(0);
      out << (quint16)(block.size() - sizeof(quint16));

      if(!tcpSock->write(block))
      {
        qDebug()<<"[zera-net-client] Failed to send message";
      }
    }
  }
}



