#ifndef TEST_SESSIONNAMESMAPPINGJSON_H
#define TEST_SESSIONNAMESMAPPINGJSON_H

#include <QObject>

class test_sessionnamesmappingjson : public QObject
{
    Q_OBJECT
private slots:
    void testMt310s2Sessions();
    void testMt310s2Com5003Sessions();
};

#endif // TEST_SESSIONNAMESMAPPINGJSON_H
