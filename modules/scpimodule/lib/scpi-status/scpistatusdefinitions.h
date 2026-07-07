#ifndef SCPISTATUSDEFINITIONS_H
#define SCPISTATUSDEFINITIONS_H

#include <QObject>

namespace SCPIStatusDefinitions {
Q_NAMESPACE

enum ScpiStatusSystems {
    questionable,
    operation,
    operationmeasure
}; // we have 1 basic and 1 cascaded status system
Q_ENUM_NS(ScpiStatusSystems)

enum ScpiStatusCommands {
    condition,
    ptransition,
    ntransition,
    event,
    enable
};
Q_ENUM_NS(ScpiStatusCommands)

}



#endif // SCPISTATUSDEFINITIONS_H
