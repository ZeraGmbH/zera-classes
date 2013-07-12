#ifndef H2012_ZERANET_SERVER_PRIVATE_H
#define H2012_ZERANET_SERVER_PRIVATE_H

#include "zeraclient.h"

/**
  @brief The Server Namespace encapsulates all networking logic into a separate namespace.
 */
namespace Zera
{
  namespace Net
  {
    class cServer;
    /**
      @brief Represents the interface between the network implementations and the ResourceManager
      */
    class cServerPrivate
    {
    protected:
      cServerPrivate();
      ~cServerPrivate();

      /**
        @brief  list of all ZeraNet::Client instances this server handles
        */
      QList<Zera::Net::cClient*> clients;

      /**
       * @brief singletonInstance
       */
      static cServer* singletonInstance;

      /**
       * @brief q_ptr See Q_DECLARE_PUBLIC
       */
      cServer *q_ptr;

    private:
      Q_DISABLE_COPY(cServerPrivate)
      Q_DECLARE_PUBLIC(cServer)
    };
  }
}
#endif // H2012_ZERANET_SERVER_PRIVATE_H
