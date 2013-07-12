#ifndef CLIENTNETBASE_PRIVATE_H
#define CLIENTNETBASE_PRIVATE_H

#include <QByteArray>

class QTcpSocket;

namespace Zera
{
  namespace NetClient
  {
    class cClientNetBase;

    /**
     * @brief The cClientNetBasePrivate class
     */
    class cClientNetBasePrivate
    {
    private:
      /**
       * @brief ClientNetBase This does nothing, look for startNetwork
       */
      cClientNetBasePrivate();

      /**
       * @brief readClient
       * @return
       */
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
