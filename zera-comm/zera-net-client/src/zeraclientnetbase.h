#ifndef CLIENTNETBASE_H
#define CLIENTNETBASE_H

#include "clientnetbase_global.h"
#include <QObject>

#include "zeraclientnetbaseprivate.h"
#include <QTcpSocket>

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
    /**
     * @brief The cClientNetBase class
     */
    class CLIENTNETBASESHARED_EXPORT cClientNetBase : public QObject
    {
      Q_OBJECT

    public:
      /**
       * @brief ClientNetBase This does nothing, look for startNetwork
       */
      explicit cClientNetBase(QObject *parent=0);


      /**
       * @brief disconnecFromServer The connection to the server will be closed
       */
      void disconnectFromServer();

      /**
        * @brief parseProtobuf Function to parse Protobuf classes from QByteArray
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
       * @brief connected Emitted when the connection is established
       */
      void connected();

      /**
       * @brief connectionLost Server is unreachable
       */
      void connectionLost();

      /**
       * @brief messageAvailable Will be called if a new message from the server arrives
       * @param message
       */
      void messageAvailable(QByteArray message);

      /**
       * @brief tcpError
       * @param errorCode
       */
      void tcpError(QAbstractSocket::SocketError errorCode);

    protected:
      /**
       * @brief d_ptr Opaque pointer
       */
      cClientNetBasePrivate *d_ptr;

    protected slots:
      /**
       * @brief newMessage Reads all messages on the socket queue
       */
      void newMessage();

    private:
      Q_DISABLE_COPY(cClientNetBase)
      Q_DECLARE_PRIVATE(cClientNetBase)

    };
  }
}

#endif // CLIENTNETBASE_H
