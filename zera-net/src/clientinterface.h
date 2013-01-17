#ifndef H2012_SERVERINTERFACE_H
#define H2012_SERVERINTERFACE_H

#include <QTcpServer>
#include "client.h"

#include "net_global.h"

/**
  @brief The Server Namespace encapsulates all networking logic into a separate namespace.
 */
namespace ZeraNet
{
  /**
    @brief Represents the interface between the network implementations and the ResourceManager
    */
  class ClientInterface : public QTcpServer
  {
    Q_OBJECT
  protected:
    /**
      @b The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
      */
    ClientInterface(QObject* parent = 0);
    ~ClientInterface();

    /**
      @b Overload the incoming connection to creater our own ZeraNet::Client
      */
    void incomingConnection(int socketDescriptor);

  public:
    /**
      @b See [P.157+ Design patterns Gang of Four]
      */
    static ClientInterface* getInstance();

  signals:


  public slots:
    /**
      @b Will be triggered when the ZeraNet::Client disconnects
      */
    void clientDisconnect();


  private:
    /**
      @b See [P.157+ Design patterns Gang of Four]
      */
    static ClientInterface* singletonInstance;
    /**
      @b  list of all ZeraNet::Client instances this server handles
      */
    QList<Client*> clients;

    /**
      @note Instances of this class should only get accessed through the getInstance method.
      */
    Q_DISABLE_COPY(ClientInterface)
  };
}
#endif // H2012_SERVERINTERFACE_H
