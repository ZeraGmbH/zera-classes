#include "taskdspdataacquisition.h"
#include "taskdecoratortimeout.h"

TaskTemplatePtr TaskDspDataAcquisition::create(const Zera::DspInterfacePtr &dspInterface,
                                               DspVarGroupClientInterface *actualValuesDsp,
                                               int limitValueCount,
                                               int timeout, std::function<void ()> additionalErrorHandler)
{
    return TaskDecoratorTimeout::wrapTimeout(timeout,
                                             std::make_unique<TaskDspDataAcquisition>(
                                                 dspInterface,
                                                 actualValuesDsp,
                                                 limitValueCount),
                                             additionalErrorHandler);
}

TaskDspDataAcquisition::TaskDspDataAcquisition(const Zera::DspInterfacePtr &dspInterface,
                                               DspVarGroupClientInterface *actualValuesDsp,
                                               int limitValueCount) :
    TaskServerTransactionTemplate(dspInterface),
    m_dspInterface(dspInterface),
    m_actualValuesDsp(actualValuesDsp),
    m_limitValueCount(limitValueCount)
{
}

quint32 TaskDspDataAcquisition::sendToServer()
{
    return m_dspInterface->dataAcquisition(m_actualValuesDsp, m_limitValueCount);
}

bool TaskDspDataAcquisition::handleCheckedServerAnswer(const QVariant &answer)
{
    Q_UNUSED(answer)
    return true;
}
