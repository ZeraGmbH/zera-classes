#ifndef PROXI_H
#define PROXI_H

#include <QObject>
#include <netmessages.pb.h>

#include "proxi_global.h"
#include "proxi_p.h"


namespace Zera
{
namespace Proxi
{

class cProxiClient;
class cProxiPrivate;

/**
  @brief The cProxi class
  */
class ZERAPROXISHARED_EXPORT cProxi: public QObject
{
    Q_OBJECT

public:
    /**
      * @brief See [P.157+ Design patterns Gang of Four]
      */
    static cProxi* getInstance();

    /**
     * @brief getConnection
     * @param ipadress
     * @param port
     * @return client over which data transfer takes place
     */
    cProxiClient* getConnection(QString ipadress, quint16 port);

    /**
     * @brief getConnection
     * @param port, ipadress is default adress
     * @return client over which data transfer takes place
     */
    cProxiClient* getConnection(quint16 port);

    /**
      * @brief setIPAdress: default adress is localhost, can be overwritten here
      * @param ipAddress
      */
    void setIPAdress(QString ipAddress);

protected:
    /**
      * @brief The class is a Singleton so the constructor is protected [P.157+ Design patterns Gang of Four]
      */
    explicit cProxi(QObject* parent = 0);
    ~cProxi();

    /**
        @b D'pointer to the private library internal structure
        this is used to hide the internal structure, and thus make the library ABI safe
      */
    cProxiPrivate *d_ptr;

private:
    Q_DISABLE_COPY(cProxi)
    Q_DECLARE_PRIVATE(cProxi)
};

}
}














#endif // PROXI_H
