#ifndef SOURCESTATECONTROLLERINTERNAL_H
#define SOURCESTATECONTROLLERINTERNAL_H

#include "sourcestates.h"
#include <QObject>

class SourceStateControllerInternal : public QObject
{
    Q_OBJECT
public:

public slots:
    void onSwitchTransactionStarted();
signals:
    void sigStateChanged(SourceStates state);

private:
    void setState(SourceStates state);

    SourceStates m_currState = SourceStates::UNDEFINED;
};

#endif // SOURCESTATECONTROLLERINTERNAL_H
