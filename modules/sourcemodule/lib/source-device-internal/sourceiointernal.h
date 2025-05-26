#ifndef SOURCEIOINTERNAL_H
#define SOURCEIOINTERNAL_H

#include "abstractsourceio.h"
#include <QJsonObject>

class SourceIoInternal : public AbstractSourceIo
{
    Q_OBJECT
public:
    SourceIoInternal(const QJsonObject capabilities);
    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    QJsonObject getCapabilities() const override;
private:
    const QJsonObject m_capabilities;
};

#endif // SOURCEIOINTERNAL_H
