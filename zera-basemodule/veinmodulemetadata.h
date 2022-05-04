#ifndef VEINMODULEMETADATA_H
#define VEINMODULEMETADATA_H

#include "metadata.h"
#include <QString>
#include <QVariant>

class cVeinModuleMetaData: public cMetaData
{
    Q_OBJECT
public:
    cVeinModuleMetaData(QString name, QVariant value);
    virtual void exportMetaData(QJsonObject &jsObj) override;
public slots:
    void setValue(QVariant value) override;
private:
    QString m_sName;
    QVariant m_vValue;
};

#endif // VEINMODULEMETADATA_H
