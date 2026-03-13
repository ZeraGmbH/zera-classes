#ifndef TEST_DSPSUPER_MODULE_H
#define TEST_DSPSUPER_MODULE_H

#include <QObject>

class test_dspsuper_module : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void minimalSession();
    void dspDumpInitial();
    void zdspDumpInitial();
    void veinDumpInitial();

};

#endif // TEST_DSPSUPER_MODULE_H
