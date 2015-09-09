#ifndef VEINMODULEMETADATA_H
#define VEINMODULEMETADATA_H

#include <QObject>
#include <QString>
#include <QVariant>

#include "metadata.h"

class QJsonArray;

class cVeinModuleMetaData: public cMetaData
{
    Q_OBJECT
public:
    cVeinModuleMetaData(QString name, QVariant value);

    virtual void exportMetaData(QJsonObject &jsObj);

public slots:
    void setValue(QVariant value);

private:
    QString m_sName;
    QVariant m_vValue;
};

#endif // VEINMODULEMETADATA_H
