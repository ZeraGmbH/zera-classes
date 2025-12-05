#include "taskoffset.h"
#include "taskoffsetgetadjcorrection.h"
#include "taskrangegetrejection.h"
#include "taskrangegeturvalue.h"
#include "errormessages.h"

TaskTemplatePtr TaskOffset::create(Zera::PcbInterfacePtr pcbInterface,
                                    QString channelMName, QString rangeName,
                                    double actualValue, double targetValue,
                                    int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler)
{
    return std::make_unique<TaskOffset>(pcbInterface,
                                        channelMName, rangeName,
                                        actualValue, targetValue,
                                        perTransactionTimout, perTransactionErrorHandler);
}

TaskOffset::TaskOffset(Zera::PcbInterfacePtr pcbInterface,
                       QString channelMName, QString rangeName, double actualValue, double targetValue,
                       int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler) :
    m_perTransactionErrorHandler(perTransactionErrorHandler)
{
    addSub(TaskOffsetGetAdjCorrection::create(pcbInterface,
                                              channelMName, rangeName, targetValue,
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
