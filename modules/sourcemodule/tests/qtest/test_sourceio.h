#ifndef QTUNITTESTSOURCEDEVICE_H
#define QTUNITTESTSOURCEDEVICE_H

#include <QObject>

class test_sourceio : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void signalResponses();
    void signalResponsesOnOneError();
    void signalResponsesOnTwoErrors();

};

#endif // QTUNITTESTSOURCEDEVICE_H
