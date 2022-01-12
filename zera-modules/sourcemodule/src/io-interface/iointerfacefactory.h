#ifndef IOINTERFACEFACTORY_H
#define IOINTERFACEFACTORY_H

#include <QSharedPointer>

enum IoInterfaceTypes
{
    SOURCE_INTERFACE_BASE = 0,
    SOURCE_INTERFACE_DEMO,
    SOURCE_INTERFACE_ASYNCSERIAL,

    SOURCE_INTERFACE_TYPE_COUNT
};

class IoInterfaceBase;
typedef QSharedPointer<IoInterfaceBase> tIoInterfaceShPtr;

// create interfaces through IoInterfaceFactory only
class IoInterfaceFactory {
public:
    static tIoInterfaceShPtr createIoInterface(IoInterfaceTypes type, QObject *parent = nullptr);
};


#endif // IOINTERFACEFACTORY_H
