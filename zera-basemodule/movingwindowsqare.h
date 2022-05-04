#ifndef MOVINGWINDOWSQARE_H
#define MOVINGWINDOWSQARE_H

#include "movingwindowfilter.h"

class cMovingwindowSqare: public cMovingwindowFilter
{
    Q_OBJECT
public:
    cMovingwindowSqare(float time);
protected:
    virtual void addnewValues();
};

#endif // MOVINGWINDOWSQARE_H
