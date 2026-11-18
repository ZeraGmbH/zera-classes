#ifndef SCPIMEASSEQUENCESTORE_H
#define SCPIMEASSEQUENCESTORE_H

#include "scpimodeldefinitions.h"
#include "scpiveincomponentsequencemeasure.h"
#include "scpiveinparser.h"
#include <QHash>

namespace SCPIMODULE {

class ScpiMeasSequenceStore
{
public:
    explicit ScpiMeasSequenceStore(const ScpiVeinParser::ScpiParseInfo &veinParseInfoMeasure);
    VeinComponentScpiMeasureSequencePtr getComponentSequence(const VeinComponentId &componentId);

    void updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue);

private:
    VeinComponentScpiMeasureSequencePtr createNewSequence(const cSCPICmdInfoPtr& scpiCmdInfo);
    static QString stringifyComponentId(const VeinComponentId &componentId);
    VeinComponentScpiMeasureSequencePtr bailOutNotFound(const VeinComponentId &componentId);

    const ScpiVeinParser::ScpiParseInfo m_veinParseInfoMeasure;
    QHash<QString /*stringifiedComponentId*/, VeinComponentScpiMeasureSequencePtr> m_componentSequences;
};

}
#endif // SCPIMEASSEQUENCESTORE_H
