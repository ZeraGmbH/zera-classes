#include "scpiclientmeasuresequencestore.h"

namespace SCPIMODULE {

VeinComponentScpiMeasureSequencePtr ScpiClientMeasureSequenceStore::getComponentSequence(const VeinComponentId &componentId,
                                                                                         const VeinComponentScpiMeasureSequence::Params &sequenceParams)
{
    auto iter = m_componentSequences.constFind(stringifyComponentId(componentId));
    if (iter != m_componentSequences.constEnd())
        return iter.value();

    VeinComponentScpiMeasureSequencePtr newSequence = std::make_shared<VeinComponentScpiMeasureSequence>(sequenceParams);
    m_componentSequences[stringifyComponentId(componentId)] = newSequence;
    return newSequence;
}

void ScpiClientMeasureSequenceStore::updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue)
{
    auto iter = m_componentSequences.constFind(stringifyComponentId(componentId));
    if (iter != m_componentSequences.constEnd())
        iter.value()->receiveMeasureValue(newValue);
}

QString ScpiClientMeasureSequenceStore::stringifyComponentId(const VeinComponentId &componentId)
{
    return QString::number(componentId.entityId) + componentId.componentName;
}

}
