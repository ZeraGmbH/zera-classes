#ifndef TEST_CHANGE_SESSION_H
#define TEST_CHANGE_SESSION_H

#include "abstractfactoryserviceinterfaces.h"
#include <QObject>

class test_change_session : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void changeToUnavailableSession();
    void changeToSameSession();

    void changeSessionMt310s2FromComponent();
    void changeSessionMt310s2SCPICmd();

    void changeSessionCom5003FromComponent();
    void changeSessionCom5003SCPICmd();

    void changeSessionMt310s2MultipleProblematicSampleModule();
    void changeSessionMt310s2MultipleProblematicRangeModule();

    void testSessionCatalogScpiCmd();
private:
    AbstractFactoryServiceInterfacesPtr m_serviceInterfaceFactory;
};

#endif // TEST_CHANGE_SESSION_H
