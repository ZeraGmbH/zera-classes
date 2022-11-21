#ifndef PROXY_H
#define PROXY_H

#include <QObject>

#include "proxy_global.h"



namespace Zera
{
namespace Proxy
{

class cProxyClient;
class cProxyPrivate;

/**
  @brief The cProxy class
  */
class ZERAPROXYSHARED_EXPORT cProxy: public QObject
{
    Q_OBJECT

public:
    /**
      * @brief See [P.127+ Design patterns Gang of Four]
      */
    static cProxy* getInstance();
    cProxyClient* getConnection(QString ipadress, quint16 port);
    cProxyClient* getConnection(quint16 port);
    void startConnection(cProxyClient *client);
    bool releaseConnection(cProxyClient* client);
    void setIPAdress(QString ipAddress);

protected:
    /**
      * @brief The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
      */
    explicit cProxy(QObject* parent = 0);
    ~cProxy();

    /**
        @b D'pointer to the private library internal structure
        this is used to hide the internal structure, and thus make the library ABI safe
      */
    cProxyPrivate *d_ptr;

private:
    Q_DISABLE_COPY(cProxy)
    Q_DECLARE_PRIVATE(cProxy)
};

}
}














#endif // PROXY_H
