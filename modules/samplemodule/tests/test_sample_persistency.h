#ifndef TEST_SAMPLE_PERSISTENCY_H
#define TEST_SAMPLE_PERSISTENCY_H

#include <QObject>

class test_sample_persistency : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void loadUL1SetUL2Reload();
    void loadPllAutoOffSetPllAutoOnReload();
};

#endif // TEST_SAMPLE_PERSISTENCY_H
