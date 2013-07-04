#ifndef H2012_ZERANET_CLIENT_PUB_H
#define H2012_ZERANET_CLIENT_PUB_H

#include <QStringList>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>
#include <QHostAddress>
#include <google/protobuf/message.h>


#include "net_global.h"

namespace ProtobufMessage
{
  class NetMessage;
}

namespace Zera
{
  namespace Net
  {
    class _ZClientPrivate;
    /**
      @brief ZeraNet::Client represents an abstract client implementation, with a timeout/refresh function
      @todo This class needs unique identifiers for implementation clients.
      */
    class ZERA_NETSHARED_EXPORT ZeraClient : public QObject
    {
      Q_OBJECT
    public:
      /**
        @brief The default constructor
        @note Other constructors are invalid
        */
      explicit ZeraClient(quint32 socketDescriptor, QString name = QString(), QObject *parent = 0);

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
        @brief returns the socket descriptor of the clients socket
        */
      quint32 getSocket();

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
       * @brief notify that the client is disconnected
       */
      void clientDisconnected();
      /**
        @brief Socket error fallback
        */
      void error(QAbstractSocket::SocketError socketError);
      /**
        @brief forwards clSocket readyRead() signal
        */
      void messageReceived(QByteArray message);


    public slots:
      /**
       * @brief Disconnects the client
       */
      void logoutClient();
      /**
       * @brief setName
       * @param newName
       */
      void setName(QString newName);
      /**
       * @brief starts the state machine
       */
      void start();



      /**
        @brief Will be called if locking the resource caused errors
        */
      void writeClient(QByteArray message);

    protected:
      Zera::Net::_ZClientPrivate* d_ptr;

    private:
      /**
        @brief Reads a QString from the socket
        */
      QByteArray readClient();
    };
  }
}
#endif // H2012_ZERANET_CLIENT_PUB_H
