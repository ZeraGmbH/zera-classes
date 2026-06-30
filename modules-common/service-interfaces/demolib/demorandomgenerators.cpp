#include "demorandomgenerators.h"
#include <QMap>

double DemoRandomGenerators::generatorFixed(int entityId)
{
    Q_UNUSED(entityId)
    return 1.0;
}

double DemoRandomGenerators::generatorReproducibleChange(int entityId)
{
    constexpr double initValue = 0.9;
    static QMap<int, double> values;
    double value = initValue;
    if (values.contains(entityId))
        value = values[entityId];
    value += 0.001;
    if (value > 1.0)
        value = initValue;
    values[entityId] = value;
    return value;
}

double DemoRandomGenerators::generatorRandom(int entityId)
{
    Q_UNUSED(entityId)
    return (double)rand() / double(RAND_MAX);
}
