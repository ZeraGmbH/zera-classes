#ifndef BLOCKEDWAITINTERFACE_H
#define BLOCKEDWAITINTERFACE_H

#include <memory>

class BlockedWaitInterface
{
public:
    enum WaitResult { WAIT_OK_SIG, WAIT_ERR_SIG, WAIT_ABORT, WAIT_TIMEOUT, WAIT_UNDEF };

    virtual bool wait() = 0;
    virtual WaitResult getResult() = 0;

    virtual ~BlockedWaitInterface() = default;
};

typedef std::unique_ptr<BlockedWaitInterface> BlockedWaitInterfacePtr;

#endif // BLOCKEDWAITINTERFACE_H
