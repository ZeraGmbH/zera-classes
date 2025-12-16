#include "taskemobwriteexchangedata.h"
#include <taskdecoratortimeout.h>
#include <hotplugcontrollerinterface.h>

TaskTemplatePtr TaskEmobWriteExchangeData::create(Zera::PcbInterfacePtr pcbInterface,
                                                  QString channelMName, int emobIdFrom0To3,
                                                  std::shared_ptr<QByteArray> exchangeDataWrite,
                                                  int timeout,
                                                  std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskEmobWriteExchangeData>(pcbInterface, channelMName, emobIdFrom0To3, exchangeDataWrite),
                                             additionalErrorHandler);
}

TaskEmobWriteExchangeData::TaskEmobWriteExchangeData(Zera::PcbInterfacePtr pcbInterface,
                                                     QString channelMName, int emobIdFrom0To3,
                                                     std::shared_ptr<QByteArray> exchangeDataWrite) :
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
    if (!(*m_exchangeDataWrite).isEmpty()) {
        QStringList sendData = HotplugControllerInterface::encodeDataToHex(*m_exchangeDataWrite);
        qInfo("Write EMOB exchange data to channel %s / hex: %s...", qPrintable(m_channelMName), qPrintable(sendData.join(",")));
        scpiCmd += sendData.join(",") + ";";
    }
    else
        qInfo("Write empty EMOB exchange data to channel %s...", qPrintable(m_channelMName));
    return m_interface->scpiCommand(scpiCmd);
}

bool TaskEmobWriteExchangeData::handleCheckedServerAnswer(QVariant answer)
{
    qInfo("EMOB exchange data to channel %s written", qPrintable(m_channelMName));
    Q_UNUSED(answer)
    return true;
}
