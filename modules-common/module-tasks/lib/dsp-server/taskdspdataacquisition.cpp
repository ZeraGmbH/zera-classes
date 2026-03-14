#include "taskdspdataacquisition.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskDspDataAcquisition::create(Zera::DspInterfacePtr dspInterface, DspVarGroupClientInterface *actualValuesDsp,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskDspDataAcquisition>(
                                                 dspInterface,
                                                 actualValuesDsp),
                                             additionalErrorHandler);

}

TaskDspDataAcquisition::TaskDspDataAcquisition(Zera::DspInterfacePtr dspInterface,
                                               DspVarGroupClientInterface *actualValuesDsp) :
    TaskServerTransactionTemplate(dspInterface),
    m_dspInterface(dspInterface),
    m_actualValuesDsp(actualValuesDsp)
{
}

quint32 TaskDspDataAcquisition::sendToServer()
{
    return m_dspInterface->dataAcquisition(m_actualValuesDsp);
}

bool TaskDspDataAcquisition::handleCheckedServerAnswer(const QVariant &answer)
{
    Q_UNUSED(answer)
    return true;
}
