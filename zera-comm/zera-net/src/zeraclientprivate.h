#ifndef H2013_ZERANET_CLIENT_H
#define H2013_ZERANET_CLIENT_H

#include <QObject>
#include <QStateMachine>
#include <QTcpSocket>
#include <QHostAddress>

#include <google/protobuf/message.h>

QT_BEGIN_NAMESPACE
class QFinalState;
class QTimer;
QT_END_NAMESPACE

namespace Zera
{
  namespace Net
  {
    class cClient;

    /**
     * @brief The cClientPrivate class
     */
    class cClientPrivate
    {

    protected:
      /**
       * @brief cClientPrivate Default constructor initializing const member
       * @param socketDescriptor Constant socket identifier
       */
      explicit cClientPrivate(quint32 socketDescriptor);

      /**
        @brief The actual socket of the Server::Client
        */
      QTcpSocket* clSocket;

      /**
       * @brief name Visual name of the client
       */
      QString name;

      /**
       * @brief socket id of the TcpSocket
       */
      const quint32 sockDescriptor;

      /**
       * @brief fstDisconnected The client disconnected
       */
      QFinalState *fstDisconnected;

      /**
       * @brief stAboutToDisconnect The client will soon disconnect
       */
      QState *stAboutToDisconnect;

      /**
       * @brief stConnected The client is authenticated and compatible
       */
      QState *stConnected;

      /**
       * @brief stContainer Container only state
       */
      QState *stContainer;

      /**
       * @brief stInit Initial state
       */
      QState *stInit;

      /**
       * @brief q_ptr see Q_DECLARE_PUBLIC
       */
      cClient* q_ptr;

    private:
      Q_DISABLE_COPY(cClientPrivate)
      Q_DECLARE_PUBLIC(cClient)
    };
  }
}
#endif // H2013_ZERANET_CLIENT_H
