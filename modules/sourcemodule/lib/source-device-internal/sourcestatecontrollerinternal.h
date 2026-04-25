#ifndef SOURCESTATECONTROLLERINTERNAL_H
#define SOURCESTATECONTROLLERINTERNAL_H

#include "sourcestates.h"
#include <QObject>

class SourceStateControllerInternal : public QObject
{
    Q_OBJECT
public:

signals:
    void sigStateChanged(SourceStates state);
};

#endif // SOURCESTATECONTROLLERINTERNAL_H
