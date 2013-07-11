#ifndef H2012_ZERANET_CLIENT_PUB_H
#define H2012_ZERANET_CLIENT_PUB_H

#include <QStringList>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>
#include <google/protobuf/message.h>

#include <QStateMachine>

#include "net_global.h"

namespace ProtobufMessage
{
  class NetMessage;
}

namespace Zera
{
  namespace Net
  {
    class cClientPrivate;
    /**
      @brief ZeraNet::Client represents an abstract client implementation, with a timeout/refresh function
      @todo This class needs unique identifiers for implementation clients.
      */
    class ZERA_NETSHARED_EXPORT cClient : public QStateMachine
    {
      Q_OBJECT
    public:
      /**
        @brief The default constructor
        @note Other constructors are invalid
        */
      explicit cClient(quint32 socketDescriptor, QString clientName = QString(), QObject *parent = 0);
      ~cClient();

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
      QByteArray translatePB2ByteArray(google::protobuf::Message *message);

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

    protected:
      cClientPrivate* d_ptr;

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
      void setupStateMachine();

      Q_DISABLE_COPY(cClient)
      Q_DECLARE_PRIVATE(cClient)
    };
  }
}
#endif // H2012_ZERANET_CLIENT_PUB_H
