#ifndef H2012_ZERANET_SERVER_PUB_H
#define H2012_ZERANET_SERVER_PUB_H

#include <QObject>
#include <QList>
#include <QTcpServer>

#include "zeraclient.h"
#include "net_global.h"

/**
  @brief The Server Namespace encapsulates all networking logic into a separate namespace.
 */
namespace Zera
{
  namespace Net
  {
    class cServerPrivate;

    /**
      @brief Represents the interface between the network implementations and the ResourceManager
      */
    class ZERA_NETSHARED_EXPORT cServer : public QTcpServer
    {
      Q_OBJECT

    signals:
      void newClientAvailable(Zera::Net::cClient* newClient);

    public:
      QList<Zera::Net::cClient*> getClients();
      /**
        @brief See [P.157+ Design patterns Gang of Four]
        */
      static cServer* getInstance();

    public slots:
      /**
       * @brief Sends a message to all clients
       * @param message
       */
      void broadcastMessage(QByteArray message);

      /**
       * @brief startServer
       * @param port
       */
      void startServer(quint16 port);

    protected:
      /**
        @brief The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
        */
      explicit cServer(QObject* parent = 0);
      ~cServer();

      /**
       * @brief incomingConnection Overloaded from QTcpServer
       * @param socketDescriptor
       */
      void incomingConnection(int socketDescriptor);

      cServerPrivate *d_ptr;

    protected slots:
      /**
       * @brief clientDisconnectedSRV A Client disconnected
       */
      void clientDisconnectedSRV();

    private:
      Q_DISABLE_COPY(cServer)
      Q_DECLARE_PRIVATE(cServer)
    };
  }
}
#endif // H2012_ZERANET_SERVER_H
