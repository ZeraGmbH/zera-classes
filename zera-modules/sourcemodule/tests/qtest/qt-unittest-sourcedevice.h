#ifndef QTUNITTESTSOURCEDEVICE_H
#define QTUNITTESTSOURCEDEVICE_H

#include <QObject>
#include "source-device/sourcedevice.h"

class SourceDeviceTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void gettersOK();
    void nonDemoInterFaceGet();
    void demoInterFaceGet();

    void disconnectSignal();

    void multipleCmdsDifferentIds();
    void observerReceiveCount();
    void observerReceiveId();

    void busySignalOnSwitch();
};

#endif // QTUNITTESTSOURCEDEVICE_H
