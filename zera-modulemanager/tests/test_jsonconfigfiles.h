#ifndef TEST_JSONCONFIGFILES_H
#define TEST_JSONCONFIGFILES_H

#include <QObject>

class test_jsonconfigfiles : public QObject
{
    Q_OBJECT
private slots:
    void compareSessionLogConfigFileCountEqual();
    void compareSessionLogConfigFileBasenamesEqual();
};

#endif // TEST_JSONCONFIGFILES_H
