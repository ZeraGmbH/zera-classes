#ifndef TEST_VEIN_STORAGE_FILTER_H
#define TEST_VEIN_STORAGE_FILTER_H

#include <testveinserver.h>
#include <QDateTime>

class test_vein_storage_filter : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void fireInitialValid();
    void noFireInitialInvalid();
    void nofireInitialValid();
    void nofireInitialValidWrongFilter();
    void fireIntitialOnceOnTwoIdenticalValid();
    void fireInitialValidOpenClose();

    void fireChangeValid();
    void fireChangeValidMultiple();
    void nofireChangeValidWrongFilter();
    void fireChangeOnceOnTwoIdenticalValid();
    void fireChangeTwoSequentialClearFilter();

    void fireSetAllSame();
private:
    void setupServer();
    QDateTime msAfterEpoch(qint64 msecs);

    std::unique_ptr<TestVeinServer> m_server;
};

#endif // TEST_VEIN_STORAGE_FILTER_H
