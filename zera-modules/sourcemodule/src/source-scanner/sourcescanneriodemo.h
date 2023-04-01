#ifndef SOURCESCANNERACTIONDEMO_H
#define SOURCESCANNERACTIONDEMO_H

#include "sourcescannertemplate.h"

class SourceScannerIoDemo : public SourceScannerTemplate
{
public:
    SourceScannerIoDemo();
private:
    virtual SourceProperties evalResponses(IoQueueGroup::Ptr ioGroup) override;
    static SupportedSourceTypes getNextSourceType();
};

#endif // SOURCESCANNERACTIONDEMO_H
