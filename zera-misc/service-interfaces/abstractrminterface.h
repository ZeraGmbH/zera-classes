#ifndef ABSTRACTRMINTERFACE_H
#define ABSTRACTRMINTERFACE_H

#include "abstractserverInterface.h"

class AbstractRmInterface : public AbstractServerInterface
{
    Q_OBJECT
public:
    virtual quint32 getResources(QString type) = 0;
    virtual quint32 getResourceTypes() = 0;
};

typedef std::shared_ptr<AbstractRmInterface> AbstractRmInterfacePtr;

#endif // ABSTRACTRMINTERFACE_H
