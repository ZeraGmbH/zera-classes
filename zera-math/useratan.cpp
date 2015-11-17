#include "math.h"

double userAtan(double im, double re)
{
    double arctan;

    if (fabs(re) < 1e-7)
        arctan = 90.0;
    else
        arctan = atan(im/re) * 57.2957795;

    if (re < 0.0)
        arctan += 180.0;

    if (arctan < 0.0)
        arctan += 360.0;

    return arctan;
}
