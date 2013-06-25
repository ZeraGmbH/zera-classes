#ifndef CLIENTNETBASE_H
#define CLIENTNETBASE_H

#include "clientnetbase_global.h"
#include <QObject>

#include "clientnetbase_private.h"

QT_BEGIN_NAMESPACE
class QTcpSocket;
QT_END_NAMESPACE

namespace google {
  namespace protobuf {
    class Message;
  }
}

namespace Zera
{
  namespace NetClient
  {

    class CLIENTNETBASESHARED_EXPORT ClientNetBase : public QObject
    {
      Q_OBJECT

    public:
      /**
       * @brief ClientNetBase This does nothing, look for startNetwork
       */
      ClientNetBase(QObject *parent);


      /**
       * @brief parseProtobuf Template function to parse Protobuf classes from QByteArray
       * @param[in,out] message Put your existing Google Protobuf implementations here
       * @param[in] array The data to parse from
       * @return True for success
       */
      static bool readMessage(google::protobuf::Message *message, const QByteArray & array);

      /**
       * @brief sendMessage Template function that sends a Protobuf message as QByteArray
       * @param[in] message Put your existing Google Protobuf object here
       */
      void sendMessage(google::protobuf::Message *message);

      /**
       * @brief startNetwork Initializes the network connection
       * @param ipAddress
       * @param port
       */
      void startNetwork(QString ipAddress, quint16 port);

    signals:
      /**
       * @brief messageAvailable Will be called if a new message from the server arrives
       * @param message
       */
      void messageAvailable(QByteArray message);
      /**
       * @brief connectionLost Server is unreachable
       */
      void connectionLost();

    private:

      /**
       * @brief d_ptr Opaque pointer
       */
      _ClientNetBasePrivate *d_ptr;


      Q_DISABLE_COPY(ClientNetBase)

    };
  }
}

#endif // CLIENTNETBASE_H
