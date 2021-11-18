#ifndef QTUNITTESTSOURCEINTERFACE_H
#define QTUNITTESTSOURCEINTERFACE_H

#include <QObject>
#include "sourceinterface.h"

class SourceInterfaceTest : public QObject
{
    Q_OBJECT
public slots:
    void onIoFinish(int ioID);

private slots:
    void init();

    void testIoIDNotSetForBaseInterface();
    void testIoIDSetForDemoInterface();
    void testDemoFinish();
    void testDemoFinishQueued();
    void testDemoFinishIDs();
    void testInterfaceTypesSetProperly();

private:
    int m_ioFinishReceived = 0;
    int m_ioIDReceived = 0;
};

#endif // QTUNITTESTSOURCEINTERFACE_H
