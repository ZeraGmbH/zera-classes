#ifndef QTUNITTESTSOURCEDEVICE_H
#define QTUNITTESTSOURCEDEVICE_H

#include <QObject>
#include "device/sourcedevice.h"

class SourceDeviceTest : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testGetters();
    void testNonDemoInterFace();
    void testDemoInterFace();

    void testDisconnect();

    void testMultipleTransactionsDiffIds();
    void testObserverReceiveCount();
    void testObserverReceiveId();
};

#endif // QTUNITTESTSOURCEDEVICE_H
