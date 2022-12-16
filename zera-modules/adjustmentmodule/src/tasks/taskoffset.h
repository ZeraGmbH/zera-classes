#ifndef TASKOFFSET_H
#define TASKOFFSET_H

#include <tasksequence.h>
#include "taskoffsetsetnode.h"
#include <pcbinterface.h>
#include <functional>

class TaskOffset : public TaskSequence
{
    Q_OBJECT
public:
    static TaskCompositePtr create(Zera::Server::PcbInterfacePtr pcbInterface,
                                   QString channelSysName, QString rangeName, double actualValue, double targetValue,
                                   int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
    TaskOffset(Zera::Server::PcbInterfacePtr pcbInterface,
               QString channelSysName, QString rangeName, double actualValue, double targetValue,
               int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
private:
    TaskOffsetSetNode::RangeVals m_rangeVals;
    std::function<void (QString)> m_perTransactionErrorHandler;
};

#endif // TASKOFFSET_H
