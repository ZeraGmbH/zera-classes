#ifndef TEST_PROCSTATDECODER_H
#define TEST_PROCSTATDECODER_H

#include <QObject>

class test_procstatdecoder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void decodeProcStatLine0();
    void decodeProcStatLine1();
    void decodeProcStat();
};

#endif
