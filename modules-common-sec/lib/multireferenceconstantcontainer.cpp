#include "multireferenceconstantcontainer.h"

MultiReferenceConstantContainer::MultiReferenceConstantContainer()
{
}

void MultiReferenceConstantContainer::initRefResources(QHash<QString, SecInputInfo> refInputInfoHash)
{
    m_refInputInfoHash = refInputInfoHash;
}

void MultiReferenceConstantContainer::catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer)
{
    if(!m_refInputInfoHash.isEmpty()) {

    }
}

void MultiReferenceConstantContainer::startSetupTasks()
{
    emit sigSetupOk();

}
