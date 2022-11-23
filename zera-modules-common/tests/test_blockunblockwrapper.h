#ifndef TEST_BLOCKUNBLOCKWRAPPER_H
#define TEST_BLOCKUNBLOCKWRAPPER_H

#include <QObject>
#include <QString>

class test_blockunblockwrapper : public QObject
{
    Q_OBJECT
signals:
    void sigNone();
private slots:
    void detectSignal();
    void detectSignalWithNoneErrSig();
    void handleAbort();
    void detectError();
    void detectTimeout();
};

#endif // TEST_BLOCKUNBLOCKWRAPPER_H
