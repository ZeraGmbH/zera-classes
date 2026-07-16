#ifndef SCPIMODELDEFINITIONS_H
#define SCPIMODELDEFINITIONS_H

#include <QString>

enum ScpiModelTypes {
    measure,
    configure,
    read,
    init,
    fetch
};

struct VeinComponentId {
    int entityId;
    QString componentName;
};

#endif // SCPIMODELDEFINITIONS_H
