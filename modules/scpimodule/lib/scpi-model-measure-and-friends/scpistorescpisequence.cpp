#include "scpistorescpisequence.h"

namespace SCPIMODULE {

ScpiStoreScpiSequence::ScpiStoreScpiSequence(const ScpiVeinParser::ScpiParseInfo &veinParseInfoMeasure) :
    m_veinParseInfoMeasure(veinParseInfoMeasure)
{
}

VeinComponentScpiMeasureSequencePtr ScpiStoreScpiSequence::getComponentSequence(const VeinComponentId &componentId)
{
    auto iter = m_componentSequences.constFind(stringifyComponentId(componentId));
    if (iter != m_componentSequences.constEnd())
        return iter.value();

    auto entityIter = m_veinParseInfoMeasure.constFind(componentId.entityId);
    if (entityIter == m_veinParseInfoMeasure.constEnd())
        return bailOutNotFound(componentId);

    const ScpiVeinParser::ScpiComponentParseInfo &components = entityIter.value();
    auto componentIter = components.find(componentId.componentName);
    if (componentIter == components.constEnd())
        return bailOutNotFound(componentId);

    const cSCPICmdInfoPtr& componentInfo = componentIter.value();
    VeinComponentScpiMeasureSequencePtr newSequence = std::make_shared<ScpiVeinComponentSequenceMeasure>(ScpiVeinComponentSequenceMeasure::Params{
        componentInfo->scpiModuleName,
        componentInfo->scpiCommand,
        componentInfo->veinComponentInfo
    });
    m_componentSequences[stringifyComponentId(componentId)] = newSequence;
    return newSequence;
}

void ScpiStoreScpiSequence::updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue)
{
    auto iter = m_componentSequences.constFind(stringifyComponentId(componentId));
    if (iter != m_componentSequences.constEnd())
        iter.value()->receiveMeasureValue(newValue);
}

QString ScpiStoreScpiSequence::stringifyComponentId(const VeinComponentId &componentId)
{
    return QString::number(componentId.entityId) + componentId.componentName;
}

VeinComponentScpiMeasureSequencePtr ScpiStoreScpiSequence::bailOutNotFound(const VeinComponentId &componentId)
{
    qCritical("Component sequence for entitity %i / component %s not found!",
              componentId.entityId, qPrintable(componentId.componentName));
    return nullptr;
}

}
