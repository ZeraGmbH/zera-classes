#include "signalwaiter.h"

SignalWaiter::SignalWaiter()
{
}

bool SignalWaiter::wait()
{
    if(!m_signalsConnected)
        return true;
    if(!m_signalReceived) {
        if(m_timeoutMs > 0)
            m_timeoutTimer.start(m_timeoutMs);
        m_eventLoop.exec();
    }
    return m_waitResult == WAIT_OK_SIG;
}

SignalWaiter::WaitResult SignalWaiter::getResult()
{
    return m_waitResult;
}

void SignalWaiter::abort()
{
    emit sigAbort();
}
