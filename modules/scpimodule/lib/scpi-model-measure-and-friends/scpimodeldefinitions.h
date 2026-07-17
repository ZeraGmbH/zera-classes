#ifndef SCPIMODELDEFINITIONS_H
#define SCPIMODELDEFINITIONS_H

#include <QString>

enum ScpiModelTypes {
    MEASURE,
    CONFIGURE,
    READ,
    INIT,
    FETCH
};

struct VeinComponentId {
    int entityId;
    QString componentName;
};

#endif // SCPIMODELDEFINITIONS_H
