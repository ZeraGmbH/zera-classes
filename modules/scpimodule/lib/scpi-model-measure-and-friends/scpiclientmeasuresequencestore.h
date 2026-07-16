#ifndef SCPICLIENTMEASURESEQUENCESTORE_H
#define SCPICLIENTMEASURESEQUENCESTORE_H

#include "scpimodeldefinitions.h"
#include "veincomponentscpimeasuresequence.h"
#include <QHash>

namespace SCPIMODULE {

class ScpiClientMeasureSequenceStore
{
public:
    VeinComponentScpiMeasureSequencePtr getComponentSequence(const VeinComponentId &componentId,
                                                             const VeinComponentScpiMeasureSequence::Params &sequenceParams);

    void updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue);

private:
    QString stringifyComponentId(const VeinComponentId &componentId);
    QHash<QString /*stringifiedComponentId*/, VeinComponentScpiMeasureSequencePtr> m_componentSequences;
};

}
#endif // SCPICLIENTMEASURESEQUENCESTORE_H
