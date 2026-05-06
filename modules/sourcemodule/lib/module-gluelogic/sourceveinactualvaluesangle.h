#ifndef SOURCEVEINACTUALVALUESANGLE_H
#define SOURCEVEINACTUALVALUESANGLE_H

#include <jsonparamapi.h>
#include <QObject>

class SourceVeinActualValuesAngle : public QObject
{
    Q_OBJECT
public:
    explicit SourceVeinActualValuesAngle(const QJsonObject &sourceCapabilities);
    double getPhaseDeg(phaseType type, int phaseNoBase0);
signals:
    void sigNewValues();

private:

};

#endif // SOURCEVEINACTUALVALUESANGLE_H
