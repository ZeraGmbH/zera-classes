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
    void signalResponses();
    void signalResponsesOnOneError();
    void signalResponsesOnTwoErrors();
    void signalDisconnect();

};

#endif // QTUNITTESTSOURCEDEVICE_H
