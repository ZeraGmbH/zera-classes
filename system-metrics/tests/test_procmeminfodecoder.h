#ifndef TEST_PROCMEMINFODECODER_H
#define TEST_PROCMEMINFODECODER_H

#include <QObject>

class test_procmeminfodecoder : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void decodeSingleLineMeminfo();
    void decodeTestProcMeminfo();
};

#endif // TEST_PROCMEMINFODECODER_H
