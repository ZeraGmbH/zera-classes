#ifndef VEINVALIDATORPHASESTRINGGENERATOR_H
#define VEINVALIDATORPHASESTRINGGENERATOR_H

#include <QStringList>

class VeinValidatorPhaseStringGenerator
{
public:
    static QStringList generate(int phaseCount, int maxPhaseCount=3);
};

#endif // VEINVALIDATORPHASESTRINGGENERATOR_H
