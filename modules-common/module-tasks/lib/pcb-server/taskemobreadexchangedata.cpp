#include "taskemobreadexchangedata.h"
#include <taskdecoratortimeout.h>
#include <hotplugcontrollerinterface.h>

TaskTemplatePtr TaskEmobReadExchangeData::create(Zera::PcbInterfacePtr pcbInterface,
                                                 std::shared_ptr<QByteArray> exchangeDataReceived,
                                                 QString channelMName,
                                                 int timeout,
                                                 std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskEmobReadExchangeData>(pcbInterface, exchangeDataReceived, channelMName),
                                             additionalErrorHandler);
}

TaskEmobReadExchangeData::TaskEmobReadExchangeData(Zera::PcbInterfacePtr pcbInterface,
                                                   std::shared_ptr<QByteArray> exchangeDataReceived,
                                                   QString channelMName) :
    TaskServerTransactionTemplate(pcbInterface),
    m_exchangeDataReceived(exchangeDataReceived),
    m_channelMName(channelMName)
{
}

quint32 TaskEmobReadExchangeData::sendToServer()
{
    return m_interface->scpiCommand(QString("SYSTEM:EMOB:READDATA? %1;").arg(m_channelMName));
}

bool TaskEmobReadExchangeData::handleCheckedServerAnswer(QVariant answer)
{
    *m_exchangeDataReceived = HotplugControllerInterface::decodeHexString(answer.toString());
    // assume empty OK for now
    return true;
}
