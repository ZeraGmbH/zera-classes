#ifndef VEINMODULEMETADATA_H
#define VEINMODULEMETADATA_H

#include <QString>
#include <QVariant>

class VfModuleMetaData: public QObject
{
    Q_OBJECT
public:
    VfModuleMetaData(const QString &moduleMetaEntryName, const QVariant &value);
    ~VfModuleMetaData();
    virtual void exportMetaData(QJsonObject &jsObj);
    static int getInstanceCount();
public slots:
    void setValue(QVariant value);
private:
    QString m_moduleMetaEntryName;
    QVariant m_value;
    static int m_instanceCount;
};

#endif // VEINMODULEMETADATA_H
