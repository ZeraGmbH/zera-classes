#ifndef H2012_CLIENT_PUB_H
#define H2012_CLIENT_PUB_H

#include <QStringList>
#include <QObject>
#include <QTimer>
#include <QTcpSocket>


#include "client.h"
#include "net_global.h"

namespace Zera
{
  namespace Net
  {
    /**
      @brief ZeraNet::Client represents an abstract client implementation, with a timeout/refresh function
      @todo This class needs unique identifiers for implementation clients.
      */
    class Client : public QObject
    {
      Q_OBJECT
    public:
      /**
        @b The default constructor
        @note Other constructors are invalid
        */
      explicit Client(int socketDescriptor, QObject *parent = 0);

      /**
        @b returns the socket descriptor of the clients socket
        */
      int getSocket();

      /**
        @b Reads a QString from the socket
        */
      QString readClient();

    signals:

      /**
        @b forwards clSocket readyRead() signal
        */
      void readyRead();
      /**
        @b Socket error fallback
        */
      void error(QTcpSocket::SocketError socketError);
      /**
        @brief timeout raises when no communication between server and client occurs within 5 seconds
        */
      void timeout();

    private:
      /**
        @b Writes a QString to the socket
        */
      void writeClient(QString message);

      Zera::Net::_ClientPrivate* d_ptr;


    public slots:
      /**
        @brief Will be called if locking the resource caused errors
        */
      void sendToClient(QString message);

      /**
        @brief Resets the timer back to 5 seconds.
        */
      void refresh();
    };
  }
}
#endif // H2012_CLIENT_PUB_H
