#include "taskemobwriteexchangedata.h"
#include <taskdecoratortimeout.h>
#include <hotplugcontrollerinterface.h>

TaskTemplatePtr TaskEmobWriteExchangeData::create(Zera::PcbInterfacePtr pcbInterface,
                                                  QByteArray exchangeDataWrite,
                                                  QString channelMName,
                                                  int timeout,
                                                  std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskEmobWriteExchangeData>(pcbInterface, exchangeDataWrite, channelMName),
                                             additionalErrorHandler);
}

TaskEmobWriteExchangeData::TaskEmobWriteExchangeData(Zera::PcbInterfacePtr pcbInterface,
                                                     QByteArray exchangeDataWrite,
                                                     QString channelMName) :
    TaskServerTransactionTemplate(pcbInterface),
    m_exchangeDataWrite(exchangeDataWrite),
    m_channelMName(channelMName)
{
}

quint32 TaskEmobWriteExchangeData::sendToServer()
{
    const QString paramData = HotplugControllerInterface::encodeDataToHex(m_exchangeDataWrite).join(",");
    const QString scpiCmd = QString("SYSTEM:EMOB:WRITEDATA %1;%2;").arg(m_channelMName, paramData);
    return m_interface->scpiCommand(scpiCmd);
}

bool TaskEmobWriteExchangeData::handleCheckedServerAnswer(QVariant answer)
{
    Q_UNUSED(answer)
    return true;
}
