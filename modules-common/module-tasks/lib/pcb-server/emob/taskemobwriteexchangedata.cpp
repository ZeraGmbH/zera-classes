#include "taskemobwriteexchangedata.h"
#include <taskdecoratortimeout.h>
#include <hotplugcontrollerinterface.h>

TaskTemplatePtr TaskEmobWriteExchangeData::create(Zera::PcbInterfacePtr pcbInterface,
                                                  QString channelMName, int emobIdFrom0To3,
                                                  QByteArray exchangeDataWrite,
                                                  int timeout,
                                                  std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskEmobWriteExchangeData>(pcbInterface, channelMName, emobIdFrom0To3, exchangeDataWrite),
                                             additionalErrorHandler);
}

TaskEmobWriteExchangeData::TaskEmobWriteExchangeData(Zera::PcbInterfacePtr pcbInterface,
                                                     QString channelMName, int emobIdFrom0To3,
                                                     QByteArray exchangeDataWrite) :
    TaskServerTransactionTemplate(pcbInterface),
    m_channelMName(channelMName),
    m_emobIdFrom0To3(emobIdFrom0To3),
    m_exchangeDataWrite(exchangeDataWrite)
{
}

quint32 TaskEmobWriteExchangeData::sendToServer()
{
    QString scpiCmd = QString("SYSTEM:EMOB:WRITEDATA %1;%2;").
                            arg(m_channelMName).arg(m_emobIdFrom0To3);
    if (!m_exchangeDataWrite.isEmpty())
        scpiCmd += HotplugControllerInterface::encodeDataToHex(m_exchangeDataWrite).join(",") + ";";
    return m_interface->scpiCommand(scpiCmd);
}

bool TaskEmobWriteExchangeData::handleCheckedServerAnswer(QVariant answer)
{
    Q_UNUSED(answer)
    return true;
}
