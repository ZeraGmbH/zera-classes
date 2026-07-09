#ifndef SCPIMODELBASE_H
#define SCPIMODELBASE_H

#include <QObject>

namespace SCPIMODULE
{

class ScpiModelBase : public QObject
{
    Q_OBJECT
public:
    ScpiModelBase();
    virtual ~ScpiModelBase();

    static int getInstanceCount();

private:
    static int m_interfaceCount;
};

}

#endif // SCPIMODELBASE_H
