#include "taskrefpowerfetchconstant.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskRefPowerFetchConstant::create(Zera::PcbInterfacePtr pcbInterface,
                                                  QString refPowerName,
                                                  std::shared_ptr<double> refPowerConstantReceived,
                                                  int timeout,
                                                  std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskRefPowerFetchConstant>(
                                                 pcbInterface,
                                                 refPowerName,
                                                 refPowerConstantReceived),
                                             additionalErrorHandler);

}

TaskRefPowerFetchConstant::TaskRefPowerFetchConstant(Zera::PcbInterfacePtr pcbInterface,
                                                 QString refPowerName,
                                                 std::shared_ptr<double> refPowerConstantReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_refPowerName(refPowerName),
    m_refPowerConstantReceived(refPowerConstantReceived)
{
}

quint32 TaskRefPowerFetchConstant::sendToServer()
{
    return m_pcbInterface->getConstantSource(m_refPowerName);
}

bool TaskRefPowerFetchConstant::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_refPowerConstantReceived = answer.toDouble(&ok);
    return ok;
}
