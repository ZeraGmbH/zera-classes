#ifndef ABSTRACTSEMSPMFUNCTIONS_H
#define ABSTRACTSEMSPMFUNCTIONS_H

#include <QString>
#include <QHash>

class AbstractSemSpmFunctions
{
public:
    virtual ~AbstractSemSpmFunctions() = default;

    virtual const QHash<QString, double> &getUnitFactorHash() const = 0;
};

#endif // ABSTRACTSEMSPMFUNCTIONS_H
