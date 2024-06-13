#ifndef MEMORYVALUES_H
#define MEMORYVALUES_H

#include <QtGlobal>

struct MemoryValues {
    quint32 totalMemory = 0;
    quint32 freeMemory = 0;
    quint32 buffers = 0;
    quint32 cached = 0;
};

#endif // MEMORYVALUES_H
