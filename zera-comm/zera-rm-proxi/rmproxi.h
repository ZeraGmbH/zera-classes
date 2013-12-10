#ifndef RMPROXI_H
#define RMPROXI_H

#include <QObject>


#include "rmproxi_global.h"
#include "rmproxi_p.h"

namespace google {
  namespace protobuf {
    class Message;
  }
}

namespace Zera
{
  namespace RMProxi
  {

    class cInterface;

    /**
     * @brief The cRMProxi class
     */
    class RMPROXISHARED_EXPORT cRMProxi: public QObject
    {
        Q_OBJECT

    public:
        /**
          @brief See [P.157+ Design patterns Gang of Four]
          */
        static cRMProxi* getInstance();

        cInterface* getInterface();

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
      explicit cRMProxi(QObject* parent = 0);
      ~cRMProxi();

      /**
        @b D'pointer to the private library internal structure
        this is used to hide the internal structure, and thus make the library ABI safe
      */
      cRMProxiPrivate *d_ptr;

    private:
      Q_DISABLE_COPY(cRMProxi)
      Q_DECLARE_PRIVATE(cRMProxi)
    };
  }
}

#endif // RMPROXI_H
