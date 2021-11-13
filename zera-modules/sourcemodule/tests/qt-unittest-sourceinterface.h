#ifndef QTUNITTESTSOURCEINTERFACE_H
#define QTUNITTESTSOURCEINTERFACE_H

#include <QObject>
#include "sourceinterface.h"

class SourceInterfaceTest : public QObject
{
    Q_OBJECT
public slots:
    void onIoFinish(int transactionID);

private slots:
    void init();

    void testTransactionIDNotSetForBaseInterface();
    void testTransactionIDSetForDemoInterface();
    void testDemoInterfaceFinish();
    void testDemoInterfaceFinishQueued();
    void testDemoInterfaceFinishIDs();
    void testInterfaceTypesSetProperly();

private:
    int m_ioFinishReceived = 0;
    int m_transactionIDReceived = 0;
};

#endif // QTUNITTESTSOURCEINTERFACE_H
