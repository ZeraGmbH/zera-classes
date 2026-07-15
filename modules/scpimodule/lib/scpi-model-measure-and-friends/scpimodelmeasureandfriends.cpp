#include "scpimodelmeasureandfriends.h"
#include "scpimodule.h"
#include "scpidelegatexmlexportgenerator.h"
#include <scpi.h>

namespace SCPIMODULE
{

ScpiModelMeasureAndFriends::ScpiModelMeasureAndFriends(cSCPIModule* module) :
    m_pModule(module)
{
}

ScpiModelMeasureAndFriends::~ScpiModelMeasureAndFriends()
{
    m_scpiMeasureDelegateHash.clear();
    for (auto measureObject: m_measureObjectsToDelete)
        delete measureObject;
}

bool ScpiModelMeasureAndFriends::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_pModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo measureInfo = moduleInterfaces.getMeasureInfo();

    for (auto iter = measureInfo.constBegin(); iter != measureInfo.constEnd(); ++iter) {
        const QList<cSCPICmdInfoPtr> &measures = iter.value();
        for (const cSCPICmdInfoPtr &measure : measures)
            addSCPICommand(scpiInterface, measure);
    }
    return true;
}

QHash<QString, MeasureScpiNodeDelegatePtr> *ScpiModelMeasureAndFriends::getSCPIMeasDelegateHash()
{
    return &m_scpiMeasureDelegateHash;
}

void ScpiModelMeasureAndFriends::updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue)
{
    const QList<VeinComponentScpiMeasureSequence*> scpiMeasureList = VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->values(componentName);
    for(int i = 0; i < scpiMeasureList.count(); i++) {
        VeinComponentScpiMeasureSequence *scpiMeasure = scpiMeasureList.at(i);
        if(scpiMeasure->entityID() == entityId)
            scpiMeasure->receiveMeasureValue(newValue);
    }
}

void ScpiModelMeasureAndFriends::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    // in case of measure model we have to add several commands for each value
    VeinComponentScpiMeasureSequence* measureObject = new VeinComponentScpiMeasureSequence(scpiCmdInfo);
    m_measureObjectsToDelete.append(measureObject);

    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "MEASURE", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "CONFIGURE", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "READ", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "INIT", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"", "FETCH", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"MEASURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"CONFIGURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"READ", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"INIT", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{"FETCH", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("MEASURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::measure, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("CONFIGURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::configure, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("READ:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::read, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("INIT:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::init, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasure::Params{QString("FETCH:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::fetch, measureObject},
                          scpiCmdInfo->veinComponentInfo);
}

void ScpiModelMeasureAndFriends::addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                                                       const ScpiDelegateMeasure::Params &params,
                                                       const QJsonObject &veinComponentInfo)
{
    QString cmdcomplete = QString("%1:%2").arg(params.cmdParent, params.cmd);
    MeasureScpiNodeDelegatePtr delegate;
    if (m_scpiMeasureDelegateHash.contains(cmdcomplete)) {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addVeinComponentScpiSequence(params.scpiMeasureObject);
    }
    else {
        delegate = std::make_shared<ScpiDelegateMeasure>(params);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        scpiInterface->addSCPICommand(delegate);
    }
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, veinComponentInfo);
}

}
