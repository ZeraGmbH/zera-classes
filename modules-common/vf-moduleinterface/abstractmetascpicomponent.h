#ifndef ABSTRACTMETASCPICOMPONENT_H
#define ABSTRACTMETASCPICOMPONENT_H

#include <QObject>
#include <QJsonObject>

class AbstractMetaScpiComponent : public QObject
{
    Q_OBJECT
public:
    AbstractMetaScpiComponent();
    virtual ~AbstractMetaScpiComponent();

    virtual void exportMetaData(QJsonObject &jsObj) = 0;
    virtual void exportSCPIInfo(QJsonArray &jsArr) = 0;

    static int getInstanceCount();

private:
    static int m_instanceCount;
};

#endif // ABSTRACTMETASCPICOMPONENT_H
