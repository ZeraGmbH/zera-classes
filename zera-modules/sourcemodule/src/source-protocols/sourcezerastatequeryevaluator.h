#ifndef SOURCEZERASTATEQUERYEVALUATOR_H
#define SOURCEZERASTATEQUERYEVALUATOR_H

#include "io-device/ioquerycontentevaluator.h"

class SourceZeraStateQueryEvaluator : public IIoQueryContentEvaluator
{
public:
    bool evalQueryResponse(QByteArray queryContentBytes) override;
};

#endif // SOURCEZERASTATEQUERYEVALUATOR_H
