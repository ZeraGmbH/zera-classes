#ifndef VF_STORAGE_H
#define VF_STORAGE_H

#include <vf-cpp-entity.h>
#include <vf-cpp-compproxy.h>
#include <QObject>

class Vf_Storage : public QObject
{
    Q_OBJECT
public:
    explicit Vf_Storage(QObject *parent = nullptr, int entityId = 1);
    bool initOnce();
    VfCpp::VfCppEntity *getVeinEntity() const;
    void setVeinEntity(VfCpp::VfCppEntity *entity);

private:
    VfCpp::VfCppEntity *m_entity;
    bool m_isInitalized;
};

#endif // VF_STORAGE_H
