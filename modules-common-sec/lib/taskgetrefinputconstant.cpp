#include "taskgetrefinputconstant.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskGetRefInputConstant::create(Zera::PcbInterfacePtr pcbInterface,
                                                QString refInputName,
                                                std::shared_ptr<double> refContantReceived,
                                                int timeout,
                                                std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskGetRefInputConstant>(
                                                 pcbInterface,
                                                 refInputName,
                                                 refContantReceived),
                                             additionalErrorHandler);

}

TaskGetRefInputConstant::TaskGetRefInputConstant(Zera::PcbInterfacePtr pcbInterface,
                                                 QString refInputName,
                                                 std::shared_ptr<double> refConstantReceived) :
    TaskServerTransactionTemplate(pcbInterface),
    m_pcbInterface(pcbInterface),
    m_refInputName(refInputName),
    m_refConstantReceived(refConstantReceived)
{
}

quint32 TaskGetRefInputConstant::sendToServer()
{
    return m_pcbInterface->getConstantSource(m_refInputName);
}

bool TaskGetRefInputConstant::handleCheckedServerAnswer(QVariant answer)
{
    bool ok;
    *m_refConstantReceived = answer.toDouble(&ok);
    return ok;
}
