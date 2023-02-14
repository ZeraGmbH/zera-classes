#ifndef RMINTERFACE_H
#define RMINTERFACE_H

#include "service-interfaces_export.h"
#include "abstractserverInterface.h"
#include <proxyclient.h>
#include <memory>

namespace Zera {

class cRMInterfacePrivate;

class SERVICE_INTERFACES_EXPORT cRMInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cRMInterface();
    virtual ~cRMInterface();
    void setClientSmart(Zera::ProxyClientPtr client);
    virtual quint32 rmIdent(QString name);
    virtual quint32 addResource(QString type, QString name, int n, QString description, quint16 port);
    virtual quint32 removeResource(QString type, QString name);
    virtual quint32 getResourceTypes();
    virtual quint32 getResources(QString type);
    virtual quint32 getResourceInfo(QString type, QString name);
    virtual quint32 setResource(QString type, QString name, int n);
    virtual quint32 freeResource(QString type, QString name);

private:
    Q_DECLARE_PRIVATE(cRMInterface)
    cRMInterfacePrivate *d_ptr;
};

typedef std::shared_ptr<cRMInterface> RMInterfacePtr;

}

#endif // RMINTERFACE_H
