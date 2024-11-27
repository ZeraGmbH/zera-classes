#ifndef CRO_RANGEFETCHTASK_H
#define CRO_RANGEFETCHTASK_H

#include "cro_range.h"
#include <tasktemplate.h>

namespace ChannelRangeObserver {

class RangeFetchTask;
typedef std::unique_ptr<RangeFetchTask> RangeFetchTaskPtr;

class RangeFetchTask : public TaskTemplate
{
    Q_OBJECT
public:
    static RangeFetchTaskPtr create(RangePtr range);
    RangeFetchTask(RangePtr range);
    void start() override;
private slots:
    void onRangeFetched(QString channelMName, QString rangeName, bool ok);
private:
    RangePtr m_range;
};

}
#endif // CRO_RANGEFETCHTASK_H
