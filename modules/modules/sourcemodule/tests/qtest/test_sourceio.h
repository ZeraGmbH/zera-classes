#ifndef QTUNITTESTSOURCEDEVICE_H
#define QTUNITTESTSOURCEDEVICE_H

#include <QObject>
#include "sourceio.h"

class test_sourceio : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void gettersOK();
    void signalResponses();
    void signalResponsesOnOneError();
    void signalResponsesOnTwoErrors();

};

#endif // QTUNITTESTSOURCEDEVICE_H
