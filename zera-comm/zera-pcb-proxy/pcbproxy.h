#ifndef CPCBPROXY_H
#define CPCBPROXY_H

#include <QObject>

#include "pcbproxy_global.h"
#include "pcbproxy_p.h"


namespace Zera
{
namespace PCBProxy
{

class cPCBProxyPrivate;
class cInterface;

/**
  @brief The cPCBProxy class
  */
class ZERAPCBPROXYSHARED_EXPORT cPCBProxy: public QObject
{
    Q_OBJECT

public:
    /**
      * @brief See [P.157+ Design patterns Gang of Four]
      */
    static cPCBProxy* getInstance();

    cInterface* getInterface(quint16 port); // proxy has to generate network connections dynamically
    // keep in mind that caller has to delete interface, otherwise we get a lot of zombies

    /**
      * @brief setIPAdress: default pcbservers work on localhost, can be changed for testing purpose
      * @param ipAddress
      */
    void setIPAdress(QString ipAddress);

protected:
    /**
      * @brief The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
      */
    explicit cPCBProxy(QObject* parent = 0);
    ~cPCBProxy();

    /**
        @b D'pointer to the private library internal structure
        this is used to hide the internal structure, and thus make the library ABI safe
      */
    cPCBProxyPrivate *d_ptr;

private:
    Q_DISABLE_COPY(cPCBProxy)
    Q_DECLARE_PRIVATE(cPCBProxy)
};

}
}


#endif // CPCBPROXY_H
