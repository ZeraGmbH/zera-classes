#ifndef MOVINGWINDOWFILTERWITHOUTSUMFIFO_H
#define MOVINGWINDOWFILTERWITHOUTSUMFIFO_H

#include "movingwindowfilter.h"

class MovingWindowFilterWithoutSumFifo : public cMovingwindowFilter
{
    Q_OBJECT
public:
    MovingWindowFilterWithoutSumFifo(float time = 1.0);
protected:
    void addnewValues() override;
};

#endif // MOVINGWINDOWFILTERWITHOUTSUMFIFO_H
