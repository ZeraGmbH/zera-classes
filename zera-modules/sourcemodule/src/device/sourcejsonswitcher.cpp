#include "sourcejsonswitcher.h"
#include "../json/jsonstructureloader.h"
#include "../json/persistentjsonstate.h"

SourceJsonSwitcher::SourceJsonSwitcher(SourceDevice *sourceDevice) :
    SourceDeviceObserver(sourceDevice),
    m_sourceDevice(sourceDevice)
{
    SupportedSourceTypes type = sourceDevice->getType();
    QString name = sourceDevice->getName();
    QString version = sourceDevice->getVersion();
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(
                type, name, version);
    m_outInGenerator = new SourceIoPacketGenerator(paramStructure);
    m_persistentParamState = new PersistentJsonState(type, name, version);
    m_paramsCurrent.setParams(m_persistentParamState->loadJsonState());
    m_paramsRequested.setParams(m_paramsCurrent.getParams());
    connect(this, &SourceJsonSwitcher::sigBusyChangedQueued,
            this, &SourceJsonSwitcher::sigBusyChanged,
            Qt::QueuedConnection);
}

SourceJsonSwitcher::~SourceJsonSwitcher()
{
    delete m_outInGenerator;
}

void SourceJsonSwitcher::switchState(QJsonObject state)
{
    m_paramsRequested.setParams(state);
    SourceCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    SourceWorkerCmdPack workerPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    m_sourceDevice->startTransaction(workerPack);
    emit sigBusyChangedQueued(true);
}

void SourceJsonSwitcher::switchOff()
{
    m_paramsRequested.setParams(m_paramsCurrent.getParams());
    m_paramsRequested.setOn(false);
    switchState(m_paramsRequested.getParams());
}

QJsonObject SourceJsonSwitcher::getCurrParamState()
{
    return m_paramsCurrent.getParams();
}

void SourceJsonSwitcher::updateResponse(SourceWorkerCmdPack cmdPack)
{
    if(cmdPack.m_commandType == COMMAND_SWITCH) {
        emit sigBusyChanged(false);
        if(cmdPack.passedAll()) {
            m_paramsCurrent.setParams(m_paramsRequested.getParams());
            emit sigCurrParamTouched();
        }
    }
}
