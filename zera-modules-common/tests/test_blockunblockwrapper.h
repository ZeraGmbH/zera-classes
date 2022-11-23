#ifndef TEST_BLOCKUNBLOCKWRAPPER_H
#define TEST_BLOCKUNBLOCKWRAPPER_H

#include <QObject>
#include <QString>

class test_blockunblockwrapper : public QObject
{
    Q_OBJECT
signals:
    void dummyDone();
    void dummyError(QString);
private slots:
    void detectSignal();
    void detectNoSignalOnAbort();
    void detectError();
};

#endif // TEST_BLOCKUNBLOCKWRAPPER_H
