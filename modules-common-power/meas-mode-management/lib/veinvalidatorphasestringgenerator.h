#ifndef VEINVALIDATORPHASESTRINGGENERATOR_H
#define VEINVALIDATORPHASESTRINGGENERATOR_H

#include <QStringList>

// Phase mask vein validator string helper - phase mask selection related
class VeinValidatorPhaseStringGenerator
{
public:
    static QStringList generate(int phaseCount, int maxPhaseCount=3);
};

#endif // VEINVALIDATORPHASESTRINGGENERATOR_H
