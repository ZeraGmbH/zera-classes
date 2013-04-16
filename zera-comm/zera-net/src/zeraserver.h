#ifndef H2012_SERVERINTERFACE_PUB_H
#define H2012_SERVERINTERFACE_PUB_H

#include <QObject>
#include <QList>
#include "server.h"
#include "zeraclient.h"

#include "net_global.h"

/**
  @brief The Server Namespace encapsulates all networking logic into a separate namespace.
 */
namespace Zera
{
  namespace Net
  {
    /**
      @brief Represents the interface between the network implementations and the ResourceManager
      */
    class ZERA_NETSHARED_EXPORT ZeraServer : public QObject
    {
      Q_OBJECT
    public:
      QList<Zera::Net::ZeraClient*> getClients();
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static ZeraServer* getInstance();

    signals:
      void newClientAvailable(Zera::Net::ZeraClient* newClient);

    public slots:
      void broadcastMessage(QByteArray message);
      void startServer(quint16 port);

    protected:
      /**
        @b The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
        */
      ZeraServer(QObject* parent = 0);
      ~ZeraServer();

      Zera::Net::_ServerPrivate* d_ptr;
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static ZeraServer* singletonInstance;

      /**
        @note Instances of this class should only get accessed through the getInstance method.
        */
      Q_DISABLE_COPY(ZeraServer)
    };
  }
}
#endif // H2012_SERVERINTERFACE_H
