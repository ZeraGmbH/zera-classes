#include "taskoffset.h"
#include "taskoffsetgetcorrection.h"
#include "taskchannelgetrejection.h"
#include "taskchannelgeturvalue.h"
#include "errormessages.h"

TaskTemplatePtr TaskOffset::create(Zera::PcbInterfacePtr pcbInterface,
                                    QString channelSysName, QString rangeName,
                                    double actualValue, double targetValue,
                                    int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler)
{
    return std::make_unique<TaskOffset>(pcbInterface,
                                        channelSysName, rangeName,
                                        actualValue, targetValue,
                                        perTransactionTimout, perTransactionErrorHandler);
}

TaskOffset::TaskOffset(Zera::PcbInterfacePtr pcbInterface,
                       QString channelSysName, QString rangeName, double actualValue, double targetValue,
                       int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler) :
    m_perTransactionErrorHandler(perTransactionErrorHandler)
{
    addSub(TaskOffsetGetCorrection::create(pcbInterface,
                                           channelSysName, rangeName, targetValue,
                                           m_rangeVals.m_correction,
                                           perTransactionTimout, [&]{
            m_perTransactionErrorHandler(readOffsetCorrErrMsg);
        }));
    addSub(TaskChannelGetRejection::create(pcbInterface,
                                           channelSysName, rangeName,
                                           m_rangeVals.m_rejection,
                                           perTransactionTimout, [&]{
            m_perTransactionErrorHandler(readrangerejectionErrMsg);
        }));
    addSub(TaskChannelGetUrValue::create(pcbInterface,
                                         channelSysName, rangeName,
                                         m_rangeVals.m_rejectionValue,
                                         perTransactionTimout, [&]{
        m_perTransactionErrorHandler(readrangeurvalueErrMsg);
    }));
    addSub(TaskOffsetSetNode::create(pcbInterface,
                                     channelSysName, rangeName,
                                     actualValue, targetValue,
                                     m_rangeVals,
                                     perTransactionTimout, [&]{
        m_perTransactionErrorHandler(setOffsetNodeErrMsg);
    }));
}
