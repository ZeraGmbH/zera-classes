#ifndef SOURCEIOINTERNAL_H
#define SOURCEIOINTERNAL_H

#include "abstractsourceio.h"

class SourceIoInternal : public AbstractSourceIo
{
    Q_OBJECT
public:
    SourceIoInternal();
    int startTransaction(IoQueueGroup::Ptr transferGroup) override;
    SourceProperties getProperties() const override;
};

#endif // SOURCEIOINTERNAL_H
