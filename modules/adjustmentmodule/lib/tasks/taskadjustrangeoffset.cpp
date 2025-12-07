#include "taskadjustrangeoffset.h"
#include "taskgaingetadjcorrection.h"
#include "taskoffsetgetadjcorrection.h"
#include "taskrangegetrejection.h"
#include "taskrangegeturvalue.h"
#include "errormessages.h"

TaskTemplatePtr TaskAdjustRangeOffset::create(Zera::PcbInterfacePtr pcbInterface,
                                              QString channelMName, QString rangeName,
                                              double actualValue, double targetValue, const ChannelCommonStorage &channelCommon,
                                              int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler)
{
    return std::make_unique<TaskAdjustRangeOffset>(pcbInterface,
                                                   channelMName, rangeName,
                                                   actualValue, targetValue, channelCommon,
                                                   perTransactionTimout, perTransactionErrorHandler);
}

TaskAdjustRangeOffset::TaskAdjustRangeOffset(Zera::PcbInterfacePtr pcbInterface,
                                             QString channelMName, QString rangeName,
                                             double actualValue, double targetValue, const ChannelCommonStorage &channelCommon,
                                             int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler) :
    m_perTransactionErrorHandler(perTransactionErrorHandler)
{
    if (channelCommon.getInvertedPhaseState())
        actualValue = -actualValue;
    addSub(TaskGainGetAdjCorrection::create(pcbInterface,
                                            channelMName, rangeName, channelCommon.getLastGainAdjAmplitude(),
                                            m_rangeVals.m_gainAdjCorrection,
                                            perTransactionTimout, [&]{
                                                m_perTransactionErrorHandler(readGainCorrErrMsg);
                                            }));
    addSub(TaskOffsetGetAdjCorrection::create(pcbInterface,
                                              channelMName, rangeName, channelCommon.getLastOffsetAdjAmplitude(),
                                              m_rangeVals.m_offsetAdjCorrection,
                                              perTransactionTimout, [&]{
                                                  m_perTransactionErrorHandler(readOffsetCorrErrMsg);
                                              }));
    addSub(TaskRangeGetRejection::create(pcbInterface,
                                         channelMName, rangeName,
                                         m_rangeVals.m_rejection,
                                         perTransactionTimout, [&]{
                                             m_perTransactionErrorHandler(readrangerejectionErrMsg);
                                         }));
    addSub(TaskRangeGetUrValue::create(pcbInterface,
                                       channelMName, rangeName,
                                       m_rangeVals.m_urValue,
                                       perTransactionTimout, [&]{
                                           m_perTransactionErrorHandler(readrangeurvalueErrMsg);
                                       }));
    addSub(TaskOffsetSetNode::create(pcbInterface,
                                     channelMName, rangeName,
                                     actualValue, targetValue,
                                     m_rangeVals,
                                     perTransactionTimout, [&]{
                                         m_perTransactionErrorHandler(setOffsetNodeErrMsg);
                                     }));
}
