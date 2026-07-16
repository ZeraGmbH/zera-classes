#include "scpimodelmeasure.h"
#include "scpimodule.h"
#include "scpidelegatexmlexportgenerator.h"
#include <scpi.h>

namespace SCPIMODULE
{

ScpiModelMeasure::ScpiModelMeasure(cSCPIModule* module) :
    m_pModule(module)
{
}

ScpiModelMeasure::~ScpiModelMeasure()
{
    m_scpiMeasureDelegateHash.clear();
    for (auto measureObject: m_measureObjectsToDelete)
        delete measureObject;
}

void ScpiModelMeasure::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_pModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo measureInfo = moduleInterfaces.getMeasureInfo();

    for (auto iter = measureInfo.constBegin(); iter != measureInfo.constEnd(); ++iter) {
        const QList<cSCPICmdInfoPtr> &measures = iter.value();
        for (const cSCPICmdInfoPtr &measure : measures)
            addSCPICommand(scpiInterface, measure);
    }
}

QHash<QString, ScpiDelegateMeasurePtr> *ScpiModelMeasure::getSCPIMeasDelegateHash()
{
    return &m_scpiMeasureDelegateHash;
}

void ScpiModelMeasure::updatePendingMeasureSequences(int entityId, const QString &componentName, const QVariant &newValue)
{
    const QList<VeinComponentScpiMeasureSequence*> scpiMeasureList = VeinComponentScpiMeasureSequence::getVeinComponentScpiMeasureSequenceStoreSingleton()->values(componentName);
    for(int i = 0; i < scpiMeasureList.count(); i++) {
        VeinComponentScpiMeasureSequence *scpiMeasure = scpiMeasureList.at(i);
        if(scpiMeasure->entityID() == entityId)
            scpiMeasure->receiveMeasureValue(newValue);
    }
}

void ScpiModelMeasure::addSCPICommand(cSCPIInterface *scpiInterface, const cSCPICmdInfoPtr &scpiCmdInfo)
{
    VeinComponentScpiMeasureSequence* measureObject = new VeinComponentScpiMeasureSequence(scpiCmdInfo);
    m_measureObjectsToDelete.append(measureObject);

    // in case of measure model we have to add several commands for each value
    // Total root nodes (e.g MEASURE?)
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"", "MEASURE", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"", "CONFIGURE", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"", "READ", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"", "INIT", SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"", "FETCH", SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

    // module nodes (e.g MEASURE:OSC1?)
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"MEASURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::measure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"CONFIGURE", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::configure, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"READ", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::read, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"INIT", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isCmd, ScpiModelTypes::init, measureObject});
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{"FETCH", scpiCmdInfo->scpiModuleName, SCPI::isNode | SCPI::isQuery, ScpiModelTypes::fetch, measureObject});

    // single value nodes (e.g MEASURE:OSC1:UL1)
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{QString("MEASURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::measure, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{QString("CONFIGURE:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::configure, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{QString("READ:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::read, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{QString("INIT:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isCmd, ScpiModelTypes::init, measureObject},
                          scpiCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, ScpiDelegateMeasureModule::Params{QString("FETCH:%2").arg(scpiCmdInfo->scpiModuleName), scpiCmdInfo->scpiCommand, SCPI::isQuery, ScpiModelTypes::fetch, measureObject},
                          scpiCmdInfo->veinComponentInfo);
}

void ScpiModelMeasure::addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                                                       const ScpiDelegateMeasureModule::Params &params,
                                                       const QJsonObject &veinComponentInfo)
{
    QString cmdcomplete = QString("%1:%2").arg(params.cmdParent, params.cmd);
    ScpiDelegateMeasurePtr delegate;
    if (m_scpiMeasureDelegateHash.contains(cmdcomplete)) {
        delegate = m_scpiMeasureDelegateHash.value(cmdcomplete);
        delegate->addVeinComponentScpiSequence(params.scpiMeasureObject);
    }
    else {
        delegate = std::make_shared<ScpiDelegateMeasureModule>(params);
        m_scpiMeasureDelegateHash[cmdcomplete] = delegate;
        scpiInterface->addSCPICommand(delegate);
    }
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, veinComponentInfo);
}

}
