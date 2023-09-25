#ifndef VEINMODULEMETADATA_H
#define VEINMODULEMETADATA_H

#include <QString>
#include <QVariant>

class VfModuleMetaData: public QObject
{
    Q_OBJECT
public:
    VfModuleMetaData(QString name, QVariant value);
    virtual void exportMetaData(QJsonObject &jsObj);
public slots:
    void setValue(QVariant value);
private:
    QString m_sName;
    QVariant m_vValue;
};

#endif // VEINMODULEMETADATA_H
