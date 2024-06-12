#ifndef PROCMEMINFODECODER_H
#define PROCMEMINFODECODER_H

#include "memoryvalues.h"
#include <QString>

class ProcMeminfoDecoder
{
public:
    static MemoryValues getCurrentMemoryValues();
    static quint32 decodeSingleMemoryValue(QString infoLine);
private:
    static QString getProcMeminfo();
};

#endif // PROCMEMINFODECODER_H
