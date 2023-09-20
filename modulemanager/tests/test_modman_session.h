#ifndef TEST_MODMAN_SESSION_H
#define TEST_MODMAN_SESSION_H

#include <QObject>

class test_modman_session : public QObject
{
    Q_OBJECT
private slots:
    void loadModulePlugins();
    void startSession();

};

#endif // TEST_MODMAN_SESSION_H
