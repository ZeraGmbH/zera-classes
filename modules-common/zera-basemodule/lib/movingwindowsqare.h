#ifndef MOVINGWINDOWSQARE_H
#define MOVINGWINDOWSQARE_H

#include "movingwindowfilter.h"

class cMovingwindowSqare: public cMovingwindowFilter
{
    Q_OBJECT
public:
    cMovingwindowSqare(float time = 1.0);
protected:
    void addnewValues() override;
};

#endif // MOVINGWINDOWSQARE_H
