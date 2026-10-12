#ifndef SECPOWERMODULEOBSERVER_H
#define SECPOWERMODULEOBSERVER_H

#include <vs_abstractdatabase.h>

class SecPowerModuleObserver : public QObject
{
    Q_OBJECT
public:
    explicit SecPowerModuleObserver(const VeinStorage::AbstractDatabase *veinDb);

    void setPowerModulentityId(int entityId);
    int getPowerModulentityId() const;

    QString getPowerType() const;
    double getConstant() const;

signals:
    void sigPowerModuleValuesChanged();

private:
    const VeinStorage::AbstractDatabase *m_veinDb = nullptr;
    VeinStorage::AbstractComponentPtr m_measTypeComponent;
    VeinStorage::AbstractComponentPtr m_constantComponent;
    int m_currentPowerModuleEntityId = -1;
};

#endif // SECPOWERMODULEOBSERVER_H
