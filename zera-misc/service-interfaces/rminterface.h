#ifndef RMINTERFACE_H
#define RMINTERFACE_H

#include "zeramisc_export.h"
#include "abstractrminterface.h"
#include <proxyclient.h>
#include <memory>

namespace Zera { namespace Server {

class cRMInterfacePrivate;

class ZERAMISC_EXPORT cRMInterface: public AbstractRmInterface
{
    Q_OBJECT
public:
    cRMInterface();
    virtual ~cRMInterface();
    void setClientSmart(Zera::Proxy::ProxyClientPtr client);
    virtual quint32 rmIdent(QString name);
    virtual quint32 addResource(QString type, QString name, int n, QString description, quint16 port);
    virtual quint32 removeResource(QString type, QString name);
    virtual quint32 getResourceTypes();
    quint32 getResources(QString type) override;
    virtual quint32 getResourceInfo(QString type, QString name);
    virtual quint32 setResource(QString type, QString name, int n);
    virtual quint32 freeResource(QString type, QString name);

private:
    Q_DECLARE_PRIVATE(cRMInterface)
    cRMInterfacePrivate *d_ptr;
};

typedef std::shared_ptr<cRMInterface> RMInterfacePtr;

} }

#endif // RMINTERFACE_H
