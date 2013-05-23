#ifndef H2012_ZERANET_CLIENT_PUB_H
#define H2012_ZERANET_CLIENT_PUB_H

#include <QStringList>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>


#include "net_global.h"

namespace ProtobufMessage
{
  class NetMessage;
}

namespace Zera
{
  namespace Net
  {
    class _ClientPrivate;
    /**
      @brief ZeraNet::Client represents an abstract client implementation, with a timeout/refresh function
      @todo This class needs unique identifiers for implementation clients.
      */
    class ZERA_NETSHARED_EXPORT ZeraClient : public QObject
    {
      Q_OBJECT
    public:
      /**
        @b The default constructor
        @note Other constructors are invalid
        */
      explicit ZeraClient(quint32 socketDescriptor, QString name = QString(), QObject *parent = 0);

      /**
        @b returns the name of the client (something like RMS or Oscilloscope)
        */
      const QString &getName();

      /**
        @b returns the socket descriptor of the clients socket
        */
      int getSocket();

    signals:
      void clientDisconnected();
      /**
        @b Socket error fallback
        */
      void error(QAbstractSocket::SocketError socketError);
      /**
        @b forwards clSocket readyRead() signal
        */
      void messageReceived(QByteArray message);


    public slots:
      /**
       * @brief Disconnects the client
       */
      void logoutClient();
      void setName(QString newName);
      void start();



      /**
        @brief Will be called if locking the resource caused errors
        */
      void writeClient(QByteArray message);

    protected:
      Zera::Net::_ClientPrivate* d_ptr;

    private:
      /**
        @b Reads a QString from the socket
        */
      QByteArray readClient();
    };
  }
}
#endif // H2012_ZERANET_CLIENT_PUB_H
