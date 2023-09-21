#ifndef TEST_MODMAN_SESSION_H
#define TEST_MODMAN_SESSION_H

#include <QObject>

class test_modman_session : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void loadModulePluginsInstalled();
    void loadModulePluginsOE();
    void startSession();
    void changeSession();

};

#endif // TEST_MODMAN_SESSION_H
