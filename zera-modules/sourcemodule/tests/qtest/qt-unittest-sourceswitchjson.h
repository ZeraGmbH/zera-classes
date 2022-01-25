#ifndef SOURCESWITCHJSONTEST_H
#define SOURCESWITCHJSONTEST_H

#include <QObject>

class SourceSwitchJsonTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void signalSwitch();
    void signalSwitchAfterError();
    void twoSignalsSwitchSameTwice();

    void keepParamOnError();
    void changeParamOnSuccess();
};

#endif // SOURCESWITCHJSONTEST_H
