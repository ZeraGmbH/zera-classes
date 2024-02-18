#ifndef FACTORYSERVICEINTERFACESSINGLETON_H
#define FACTORYSERVICEINTERFACESSINGLETON_H

#include "abstractfactoryserviceinterfaces.h"

class FactoryServiceInterfacesSingleton
{
public:
    static void setInstance(AbstractFactoryServiceInterfacesPtr factory);
    static AbstractFactoryServiceInterfaces* getInstance();
private:
    static AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
};

#endif // FACTORYSERVICEINTERFACESSINGLETON_H
