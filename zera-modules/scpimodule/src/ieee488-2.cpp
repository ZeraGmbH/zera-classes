#include "ieee488-2.h"

namespace SCPIMODULE
{

QString cIEEE4482::RegOutput(quint8 reg)
{
    return QString("+%1").arg(reg);
}


}
