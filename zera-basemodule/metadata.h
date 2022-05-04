#ifndef METADATA_H
#define METADATA_H

#include <QJsonObject>
#include <QVariant>

// pure virtual class for meta data export

class cMetaData: public QObject
{
    Q_OBJECT
public:
    virtual void exportMetaData(QJsonObject &jsObj) = 0;
public slots:
    virtual void setValue(QVariant value) = 0;

};

#endif // METADATA_H
