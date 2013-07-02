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
    class _ZClientPrivate : public QStateMachine
    {
      Q_OBJECT
    public:
      explicit _ZClientPrivate(quint32 socketDescriptor, QString clientName = QString(), QObject *parent = 0);

      /**
       * @brief This is a forward of TcpSocket peerAddress()
       * @return IP address of the peer
       */
      QHostAddress getIpAddress();

      /**
        @brief returns the name of the client (something like RMS or Oscilloscope)
        */
      const QString &getName();

      /**
       * @brief Returns the socket descriptor of the clients socket
       * @return socket descriptor unique id
       */
      quint32 getSocket();

      /**
        @brief Reads The QByteArray from the socket
        */
      QByteArray readClient();

      /**
       * @brief translateBA2Protobuf Function to parse Protobuf messages from QByteArray
       * @param[in,out] message Put your existing Google Protobuf implementations here
       * @param[in] array The data to parse from
       * @return True for success
       */
      bool translateBA2Protobuf(google::protobuf::Message *message, const QByteArray &array);
      /**
       * @brief translatePB2ByteArray Function that wraps a Protobuf message in a QByteArray
       * @param[in] message Put your existing Google Protobuf object here
       */
      QByteArray translatePB2ByteArray(const google::protobuf::Message &message);

    signals:
      /**
       * @brief The new client connected
       */
      void clientConnected();
      /**
       * @brief The client disconnected
       */
      void clientDisconnected();
      /**
       * @brief The client will disconnect soon
       */
      void clientLogout();
      /**
       * @brief Forwards the TcpSocket error
       * @param socketError
       */
      void sockError(QAbstractSocket::SocketError socketError);
      /**
       * @brief Forwards messages to the application
       * @param the message received (UTF-8)
       */
      void messageReceived(QByteArray message);

    public slots:
      /**
       * @brief Tell the client to disconnect
       */
      void logoutClient();
      /**
       * @brief Change the clients name
       * @param newName
       */
      void setName(QString newName);
      /**
        @brief Writes a QString to the socket
        */
      void writeClient(QByteArray message);

    private slots:
      /**
       * @brief Will be called if the connection is established
       */
      void initialize();
      /**
       * @brief Will be called on readyRead
       */
      void maintainConnection();
      /**
       * @brief Shuts down the connection
       */
      void disconnectClient();

    private:
      /**
       * @brief Entry point for stated client
       */
      void setupStateMachine();
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

      Q_DISABLE_COPY(_ZClientPrivate)
    };
  }
}
#endif // H2013_ZERANET_CLIENT_H
