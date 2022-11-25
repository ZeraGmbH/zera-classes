#ifndef BLOCKEDWAITINTERFACE_H
#define BLOCKEDWAITINTERFACE_H

class BlockedWaitInterface
{
public:
    enum WaitResult { WAIT_OK_SIG, WAIT_ERR_SIG, WAIT_ABORT, WAIT_TIMEOUT, WAIT_UNDEF };

    virtual bool wait() = 0;
    virtual WaitResult getResult() = 0;
};

#endif // BLOCKEDWAITINTERFACE_H
