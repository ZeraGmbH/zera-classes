#ifndef INTERFACEENTITY_H
#define INTERFACEENTITY_H

#include <QString>

class QJsonArray;

class cInterfaceEntity
{
public:
    cInterfaceEntity(){}
    cInterfaceEntity( QString name, QString des, QString model, QString cmdnode, QString type, QString unit);

    void setName(QString name);
    void setDescription(QString des);
    void setSCPIModel(QString model);
    void setSCPICmdnode(QString node);
    void setSCPIType(QString type);
    void setUnit(QString unit);

    void appendInterfaceEntity(QJsonArray& jsArr);

private:
    QString m_sEntityName;
    QString m_sDescription;
    QString m_sSCPIModel;
    QString m_sSCPICmdnode;
    QString m_sSCPIType;
    QString m_sUnit;
};


#endif // INTERFACEENTITY_H
