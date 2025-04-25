#ifndef VEINMODULEMETADATA_H
#define VEINMODULEMETADATA_H

#include <QString>
#include <QVariant>

class VfModuleMetaData: public QObject
{
    Q_OBJECT
public:
    VfModuleMetaData(QString name, QVariant value);
    ~VfModuleMetaData();
    virtual void exportMetaData(QJsonObject &jsObj);
    static int getInstanceCount();
public slots:
    void setValue(QVariant value);
private:
    QString m_sName;
    QVariant m_vValue;
    static int m_instanceCount;
};

#endif // VEINMODULEMETADATA_H
