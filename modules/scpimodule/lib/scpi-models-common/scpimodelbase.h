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
    ~ScpiModelBase() override;

    static int getInstanceCount();

private:
    static int m_instanceCount;
};

}

#endif // SCPIMODELBASE_H
