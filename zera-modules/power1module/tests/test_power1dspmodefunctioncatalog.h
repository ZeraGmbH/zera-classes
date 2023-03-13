#ifndef TEST_POWER1DSPMODEFUNCTIONCATALOG_H
#define TEST_POWER1DSPMODEFUNCTIONCATALOG_H

#include <QObject>

class test_power1dspmodefunctioncatalog : public QObject
{
    Q_OBJECT
private slots:
    void allAvailableModesHandledByCatalog();
};

#endif // TEST_POWER1DSPMODEFUNCTIONCATALOG_H
