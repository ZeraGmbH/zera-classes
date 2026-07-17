#include "scpimodelmeasure.h"
#include "scpimodule.h"
#include "scpidelegatexmlexportgenerator.h"

namespace SCPIMODULE
{

ScpiModelMeasure::ScpiModelMeasure(cSCPIModule* module) :
    m_pModule(module)
{
}

void ScpiModelMeasure::setupScpi(cSCPIInterface *scpiInterface)
{
    const VeinScpiModuleInterfaceParser moduleInterfaces = m_pModule->getScpiModuleInterfaceParser();
    const VeinScpiModuleInterfaceParser::ScpiParseInfo measureInfo = moduleInterfaces.getMeasureInfo();

    VeinComponentByScpiPath scpiNodesAdded;
    for (auto entityIter = measureInfo.constBegin(); entityIter != measureInfo.constEnd(); ++entityIter) {
        const VeinScpiModuleInterfaceParser::ScpiComponentParseInfo components = entityIter.value();
        for (const cSCPICmdInfoPtr &component : components)
            addScpiCmdForVeinComponent(scpiInterface, scpiNodesAdded, component);
    }
}

void ScpiModelMeasure::addScpiCmdForVeinComponent(cSCPIInterface *scpiInterface,
                                                  VeinComponentByScpiPath &scpiNodesAdded,
                                                  const cSCPICmdInfoPtr &scpiComponentCmdInfo)
{
    const QString &scpiModuleName = scpiComponentCmdInfo->scpiModuleName;
    const QString &scpiCommand = scpiComponentCmdInfo->scpiCommand;
    const VeinComponentId componentId = scpiComponentCmdInfo->componentId();
    const QJsonObject &veinComponentInfo = scpiComponentCmdInfo->veinComponentInfo;
    constexpr quint8 SCPI_BITMASK_QUERY = SCPI::isQuery;
    constexpr quint8 SCPI_BITMASK_CMD = SCPI::isCmd;
    constexpr quint8 SCPI_BITMASK_NODE_QUERY = SCPI::isNode | SCPI::isQuery;
    constexpr quint8 SCPI_BITMASK_NODE_CMD = SCPI::isNode | SCPI::isCmd;

    // in case of measure model we have to add several commands for each value
    // Total root nodes (e.g MEASURE?)
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"", "MEASURE", SCPI_BITMASK_NODE_QUERY, MEASURE, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"", "CONFIGURE", SCPI_BITMASK_NODE_CMD, CONFIGURE, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"", "READ", SCPI_BITMASK_NODE_QUERY, READ, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"", "INIT", SCPI_BITMASK_NODE_CMD, INIT, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"", "FETCH", SCPI_BITMASK_NODE_QUERY, FETCH, QJsonObject()},
                          componentId);

    // module nodes (e.g MEASURE:OSC1?)
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"MEASURE", scpiModuleName, SCPI_BITMASK_NODE_QUERY, MEASURE, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"CONFIGURE", scpiModuleName, SCPI_BITMASK_NODE_CMD, CONFIGURE, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"READ", scpiModuleName, SCPI_BITMASK_NODE_QUERY, READ, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"INIT", scpiModuleName, SCPI_BITMASK_NODE_CMD, INIT, QJsonObject()},
                          componentId);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{"FETCH", scpiModuleName, SCPI_BITMASK_NODE_QUERY, FETCH, QJsonObject()},
                          componentId);

    // single value nodes (e.g MEASURE:OSC1:UL1)
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{QString("MEASURE:%2").arg(scpiModuleName), scpiCommand, SCPI_BITMASK_QUERY, MEASURE, veinComponentInfo},
                          componentId,
                          scpiComponentCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{QString("CONFIGURE:%2").arg(scpiModuleName), scpiCommand, SCPI_BITMASK_CMD, CONFIGURE, veinComponentInfo},
                          componentId,
                          scpiComponentCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{QString("READ:%2").arg(scpiModuleName), scpiCommand, SCPI_BITMASK_QUERY, READ, veinComponentInfo},
                          componentId,
                          scpiComponentCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{QString("INIT:%2").arg(scpiModuleName), scpiCommand, SCPI_BITMASK_CMD, INIT, veinComponentInfo},
                          componentId,
                          scpiComponentCmdInfo->veinComponentInfo);
    addSCPIMeasureCommand(scpiInterface, scpiNodesAdded,
                          ScpiDelegateMeasure::Params{QString("FETCH:%2").arg(scpiModuleName), scpiCommand, SCPI_BITMASK_QUERY, FETCH, veinComponentInfo},
                          componentId,
                          scpiComponentCmdInfo->veinComponentInfo);
}

void ScpiModelMeasure::addSCPIMeasureCommand(cSCPIInterface *scpiInterface,
                                             VeinComponentByScpiPath &scpiNodesAdded,
                                             const ScpiDelegateMeasure::Params &params,
                                             const VeinComponentId &componentId,
                                             const QJsonObject &veinComponentInfo)
{
    QString cmdcomplete = QString("%1:%2").arg(params.cmdParent, params.cmd);
    ScpiDelegateMeasurePtr delegate;
    if (scpiNodesAdded.contains(cmdcomplete))
        // all vein components pass here for e.g root nodes as MEASURE
        delegate = scpiNodesAdded.value(cmdcomplete);
    else {
        delegate = std::make_shared<ScpiDelegateMeasure>(params);
        scpiNodesAdded[cmdcomplete] = delegate;
        scpiInterface->addSCPICommand(delegate);
    }
    delegate->addComponentId(componentId);
    ScpiDelegateXmlExportGenerator::setXmlComponentInfo(delegate, veinComponentInfo);
}

}
