#ifndef H2012_ZERANET_SERVER_PUB_H
#define H2012_ZERANET_SERVER_PUB_H

#include <QObject>
#include <QList>

#include "zeraclient.h"
#include "net_global.h"

/**
  @brief The Server Namespace encapsulates all networking logic into a separate namespace.
 */
namespace Zera
{
  namespace Net
  {
    class _ZServerPrivate;

    /**
      @brief Represents the interface between the network implementations and the ResourceManager
      */
    class ZERA_NETSHARED_EXPORT ZeraServer : public QObject
    {
      Q_OBJECT
    public:
      QList<Zera::Net::ZeraClient*> getClients();
      /**
        @brief See [P.157+ Design patterns Gang of Four]
        */
      static ZeraServer* getInstance();

    signals:
      void newClientAvailable(Zera::Net::ZeraClient* newClient);

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
      ZeraServer(QObject* parent = 0);
      ~ZeraServer();

      Zera::Net::_ZServerPrivate* d_ptr;
      /**
        @brief See [P.157+ Design patterns Gang of Four]
        */
      static ZeraServer* singletonInstance;

      /**
        @note Instances of this class should only get accessed through the getInstance method.
        */
      Q_DISABLE_COPY(ZeraServer)
    };
  }
}
#endif // H2012_ZERANET_SERVER_H
