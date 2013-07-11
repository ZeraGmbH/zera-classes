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

    class cClientPrivate
    {

    protected:
      explicit cClientPrivate(quint32 socketDescriptor);

      /**
        @brief The actual socket of the Server::Client
        */
      QTcpSocket* clSocket;

      QString name;
      /**
       * @brief socket id of the TcpSocket
       */
      const quint32 sockDescriptor;

      QFinalState *fstDisconnected;
      QState *stAboutToDisconnect;
      QState *stConnected;
      QState *stContainer;
      QState *stInit;

      cClient* q_ptr;

    private:
      Q_DISABLE_COPY(cClientPrivate)
      Q_DECLARE_PUBLIC(cClient)
    };
  }
}
#endif // H2013_ZERANET_CLIENT_H
