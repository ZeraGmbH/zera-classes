#ifndef H2012_SERVERINTERFACE_H
#define H2012_SERVERINTERFACE_H

#include <QTcpServer>
#include "client.h"

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
    class _ClientInterfacePrivate : public QTcpServer
    {
      Q_OBJECT
    protected:
      /**
        @b The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
        */
      _ClientInterfacePrivate(QObject* parent = 0);
      ~_ClientInterfacePrivate();

      /**
        @b Overload the incoming connection to creater our own ZeraNet::Client
        */
      void incomingConnection(int socketDescriptor);

    public:
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static _ClientInterfacePrivate* getInstance();


    public slots:
      /**
        @b Will be triggered when the ZeraNet::Client disconnects
        */
      void clientDisconnect();


    private:
      /**
        @b See [P.157+ Design patterns Gang of Four]
        */
      static _ClientInterfacePrivate* singletonInstance;
      /**
        @b  list of all ZeraNet::Client instances this server handles
        */
      QList<_ClientPrivate*> clients;

      /**
        @note Instances of this class should only get accessed through the getInstance method.
        */
      Q_DISABLE_COPY(_ClientInterfacePrivate)
    };
  }
}
#endif // H2012_SERVERINTERFACE_H
