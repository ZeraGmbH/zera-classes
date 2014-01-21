#ifndef RMPROXY_H
#define RMPROXY_H

#include <QObject>


#include "rmproxy_global.h"
#include "rmproxy_p.h"

namespace google {
  namespace protobuf {
    class Message;
  }
}

namespace Zera
{
  namespace RMProxy
  {

    class cInterface;

    /**
     * @brief The cRMProxy class
     */
    class RMPROXYSHARED_EXPORT cRMProxy: public QObject
    {
        Q_OBJECT

    public:
        /**
          @brief See [P.157+ Design patterns Gang of Four]
          */
        static cRMProxy* getInstance();

        cInterface* getInterface();
        // keep in mind that caller has to delete interface, otherwise we get a lot of zombies
        /**
         * @brief startNetwork Initializes the network connection and establishes rm connection
         * @param ipAddress
         * @param port
         */
        void startNetwork(QString ipAddress, quint16 port);

    protected:
      /**
        @brief The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
        */
      explicit cRMProxy(QObject* parent = 0);
      ~cRMProxy();

      /**
        @b D'pointer to the private library internal structure
        this is used to hide the internal structure, and thus make the library ABI safe
      */
      cRMProxyPrivate *d_ptr;

    private:
      Q_DISABLE_COPY(cRMProxy)
      Q_DECLARE_PRIVATE(cRMProxy)
    };
  }
}

#endif // RMPROXY_H
