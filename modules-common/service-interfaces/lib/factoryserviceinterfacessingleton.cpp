#include "factoryserviceinterfacessingleton.h"

AbstractFactoryServiceInterfacesPtr FactoryServiceInterfacesSingleton::m_serviceInterfaceFactory;

void FactoryServiceInterfacesSingleton::setInstance(AbstractFactoryServiceInterfacesPtr factory)
{
    Q_ASSERT(factory);
    Q_ASSERT(!m_serviceInterfaceFactory);
    m_serviceInterfaceFactory = std::move(factory);
}

AbstractFactoryServiceInterfaces *FactoryServiceInterfacesSingleton::getInstance()
{
    Q_ASSERT(m_serviceInterfaceFactory);
    return m_serviceInterfaceFactory.get();
}
