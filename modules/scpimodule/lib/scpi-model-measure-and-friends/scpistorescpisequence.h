#ifndef SCPISTORESCPISEQUENCE_H
#define SCPISTORESCPISEQUENCE_H

#include "scpimodeldefinitions.h"
#include "scpiveincomponentsequencemeasure.h"
#include "veinscpimoduleinterfaceparser.h"
#include <QHash>

namespace SCPIMODULE {

class ScpiStoreScpiSequence
{
public:
    ScpiStoreScpiSequence(const VeinScpiModuleInterfaceParser::ScpiParseInfo &veinParseInfoMeasure);
    VeinComponentScpiMeasureSequencePtr getComponentSequence(const VeinComponentId &componentId);

    void updatePendingMeasureSequences(const VeinComponentId &componentId, const QVariant &newValue);

private:
    QString stringifyComponentId(const VeinComponentId &componentId);
    VeinComponentScpiMeasureSequencePtr bailOutNotFound(const VeinComponentId &componentId);

    const VeinScpiModuleInterfaceParser::ScpiParseInfo m_veinParseInfoMeasure;
    QHash<QString /*stringifiedComponentId*/, VeinComponentScpiMeasureSequencePtr> m_componentSequences;
};

}
#endif // SCPISTORESCPISEQUENCE_H
