#ifndef MULTIREFERENCECONSTANTCONTAINER_H
#define MULTIREFERENCECONSTANTCONTAINER_H

#include "secinputinfo.h"
#include <QObject>
#include <QHash>
#include <QVariant>

class MultiReferenceConstantContainer : public QObject
{
    Q_OBJECT
public:
    MultiReferenceConstantContainer();
    void initRefResources(QHash<QString, SecInputInfo> refInputInfoHash);
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void startSetupTasks();
signals:
    void sigSetupOk();
    void sigSetupErr();
private:
    QHash<QString, SecInputInfo> m_refInputInfoHash;
};

#endif // MULTIREFERENCECONSTANTCONTAINER_H
