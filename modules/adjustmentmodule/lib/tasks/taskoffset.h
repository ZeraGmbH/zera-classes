#ifndef TASKOFFSET_H
#define TASKOFFSET_H

#include <taskcontainersequence.h>
#include "taskoffsetsetnode.h"
#include <pcbinterface.h>
#include <functional>

class TaskOffset : public TaskContainerSequence
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                   QString channelMName, QString rangeName, double actualValue, double targetValue,
                                   int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
    TaskOffset(Zera::PcbInterfacePtr pcbInterface,
               QString channelMName, QString rangeName, double actualValue, double targetValue,
               int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
private:
    TaskOffsetSetNode::RangeWorkStorage m_rangeVals;
    std::function<void (QString)> m_perTransactionErrorHandler;
};

#endif // TASKOFFSET_H
