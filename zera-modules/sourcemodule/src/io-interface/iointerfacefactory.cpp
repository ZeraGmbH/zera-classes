#include "iointerfacefactory.h"
#include "iointerfacebase.h"
#include "iointerfacedemo.h"
#include "iointerfacezeraserial.h"

tIoInterfaceShPtr IoInterfaceFactory::createIoInterface(IoInterfaceTypes type, QObject *parent)
{
    IoInterfaceBase* interface = nullptr;
    switch (type) {
    case SOURCE_INTERFACE_BASE:
        interface = new IoInterfaceBase(parent);
        break;
    case SOURCE_INTERFACE_DEMO:
        interface = new IoInterfaceDemo(parent);
        break;
    case SOURCE_INTERFACE_ASYNCSERIAL:
        interface = new IoInterfaceZeraSerial(parent);
        break;
    case SOURCE_INTERFACE_TYPE_COUNT:
        qCritical("Do not use SOURCE_INTERFACE_TYPE_COUNT");
        break;
    }
    return tIoInterfaceShPtr(interface);
}
