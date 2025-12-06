#ifndef TASKADJUSTRANGEOFFSET_H
#define TASKADJUSTRANGEOFFSET_H

#include <taskcontainersequence.h>
#include "channeladjstorage.h"
#include "taskoffsetsetnode.h"
#include <pcbinterface.h>
#include <functional>

class TaskAdjustRangeOffset : public TaskContainerSequence
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  QString channelMName, QString rangeName,
                                  double actualValue, double targetValue, const ChannelAdjStorage &adjStorage,
                                  int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
    TaskAdjustRangeOffset(Zera::PcbInterfacePtr pcbInterface,
                          QString channelMName, QString rangeName,
                          double actualValue, double targetValue, const ChannelAdjStorage &adjStorage,
                          int perTransactionTimout, std::function<void (QString)> perTransactionErrorHandler);
private:
    TaskOffsetSetNode::RangeWorkStorage m_rangeVals;
    std::function<void (QString)> m_perTransactionErrorHandler;
};

#endif // TASKADJUSTRANGEOFFSET_H
