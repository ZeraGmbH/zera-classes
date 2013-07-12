#ifndef CLIENTNETBASE_PRIVATE_H
#define CLIENTNETBASE_PRIVATE_H


#include <QObject>
#include <QTcpSocket>


namespace google {
  namespace protobuf {
    class Message;
  }
}


namespace Zera
{
  namespace NetClient
  {
    class cClientNetBase;

    class cClientNetBasePrivate
    {
    private:
      /**
       * @brief ClientNetBase This does nothing, look for startNetwork
       */
      cClientNetBasePrivate();

      QByteArray readClient();
      /**
       * @brief sendByteArray
       * @param bA Data that will be sent
       */
      void sendByteArray(const QByteArray &bA);

      /**
       * @brief tcpSock
       */
      QTcpSocket* tcpSock;

      /**
       * @brief q_ptr
       */
      cClientNetBase *q_ptr;

      Q_DECLARE_PUBLIC(cClientNetBase)
    };
  }
}

#endif // CLIENTNETBASE_PRIVATE_H
