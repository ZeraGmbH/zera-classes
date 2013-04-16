#ifndef H2012_SERVERINTERFACE_H
#define H2012_SERVERINTERFACE_H

#include <QTcpServer>
#include "zeraclient.h"

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
    class _ServerPrivate : public QTcpServer
    {
      Q_OBJECT
    public:
      QList<Zera::Net::ZeraClient*> getClients();
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static _ServerPrivate* getInstance();

    signals:
      void newClientAvailable(Zera::Net::ZeraClient* newClient);

    public slots:
      void broadcastMessage(QByteArray message);
      /**
        @b Will be triggered when the ZeraNet::Client disconnects
        */
      void clientDisconnected();

      void startServer(quint16 port);

    protected:
      /**
        @b The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
        */
      _ServerPrivate(QObject* parent = 0);
      ~_ServerPrivate();

      /**
        @b Overload the incoming connection to creater our own ZeraNet::Client
        */
      void incomingConnection(int socketDescriptor);
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static _ServerPrivate* singletonInstance;

    private:
      /**
        @b  list of all ZeraNet::Client instances this server handles
        */
      QList<Zera::Net::ZeraClient*> clients;

      /**
        @note Instances of this class should only get accessed through the getInstance method.
        */
      Q_DISABLE_COPY(_ServerPrivate)
    };
  }
}
#endif // H2012_SERVERINTERFACE_H
