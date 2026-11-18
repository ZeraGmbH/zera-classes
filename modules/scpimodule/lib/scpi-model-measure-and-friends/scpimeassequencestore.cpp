#include "scpimeassequencestore.h"

namespace SCPIMODULE {

ScpiMeasSequenceStore::ScpiMeasSequenceStore(const ScpiVeinParser::ScpiParseInfo &veinParseInfoMeasure) :
    m_veinParseInfoMeasure(veinParseInfoMeasure)
{
}

VeinComponentScpiMeasureSequencePtr ScpiMeasSequenceStore::getComponentSequence(const VeinComponentId &componentId)
{
    const QString componentKey = stringifyComponentId(componentId);

    // Do we have already a component sequence for component?
    auto iter = m_componentSequences.constFind(componentKey);
    if (iter != m_componentSequences.constEnd())
        return iter.value();

    // Known entity?
    auto entityIter = m_veinParseInfoMeasure.constFind(componentId.entityId);
    if (entityIter == m_veinParseInfoMeasure.constEnd())
        return bailOutNotFound(componentId);

    // Known component?
    const ScpiVeinParser::ScpiComponentParseInfo &components = entityIter.value();
    auto componentIter = components.find(componentId.componentName);
    if (componentIter == components.constEnd())
        return bailOutNotFound(componentId);

    VeinComponentScpiMeasureSequencePtr newMeasSequence = createNewSequence(componentIter.value());
    m_componentSequences[componentKey] = newMeasSequence;
    return newMeasSequence;
}

void ScpiMeasSequenceStore::updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue)
{
    const QString componentKey = stringifyComponentId(componentId);
    auto iter = m_componentSequences.constFind(componentKey);
    if (iter != m_componentSequences.constEnd())
        iter.value()->receiveMeasureValue(newValue);
}

VeinComponentScpiMeasureSequencePtr ScpiMeasSequenceStore::createNewSequence(const cSCPICmdInfoPtr &scpiCmdInfo)
{
    VeinComponentScpiMeasureSequencePtr newSequence = std::make_shared<ScpiVeinComponentSequenceMeasure>(ScpiVeinComponentSequenceMeasure::Params{
        scpiCmdInfo->scpiModuleName,
        scpiCmdInfo->scpiCommand,
        scpiCmdInfo->veinComponentInfo
    });
    return newSequence;
}

QString ScpiMeasSequenceStore::stringifyComponentId(const VeinComponentId &componentId)
{
    return QString::number(componentId.entityId) + componentId.componentName;
}

VeinComponentScpiMeasureSequencePtr ScpiMeasSequenceStore::bailOutNotFound(const VeinComponentId &componentId)
{
    qCritical("Component sequence for entitity %i / component %s not found!",
              componentId.entityId, qPrintable(componentId.componentName));
    return nullptr;
}

}
