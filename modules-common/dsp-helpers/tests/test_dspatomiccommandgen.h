#ifndef TEST_DSPATOMICCOMMANDGEN_H
#define TEST_DSPATOMICCOMMANDGEN_H

#include <QObject>

class test_dspatomiccommandgen : public QObject
{
    Q_OBJECT
private slots:
    void startChain();
    void stopChain();
    void activateChain();
    void deactivateChain();
};

#endif // TEST_DSPATOMICCOMMANDGEN_H
