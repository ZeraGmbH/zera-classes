#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include "blockedwaitinterface.h"
#include <QObject>
#include <QEventLoop>
#include <QTimer>

/**
 * @brief A class for blocked wait on signals. It enables structured code for
 * simple sequences e.g as an alternate using QStateMachines which creates
 * uneasy to read and unflexible code
 */
class SignalWaiter : public QObject, public BlockedWaitInterface
{
    Q_OBJECT
public:
    SignalWaiter();
    template <typename Func1>
    SignalWaiter(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                 int timeout = 0)
    {
        setup(doneSender, doneSignal, this, &SignalWaiter::sigDefaultNoError, timeout);
    }
    template <typename Func1, typename Func2>
    SignalWaiter(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                 const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal,
                 int timeout = 0)
    {
        setup(doneSender, doneSignal, errorSender, errorSignal, timeout);
    }
    bool wait() override;
    WaitResult getResult() override;
    void abort();
signals:
    void sigDefaultNoError();
    void sigAbort();
private:
    template <typename Func1, typename Func2>
    void setup(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
               const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal,
               int timeout = 0)
    {
        m_signalsConnected = true;
        m_timeoutMs = timeout;
        connect(doneSender, doneSignal, this, [&]() {
            m_signalReceived = true;
            m_waitResult = WAIT_OK_SIG;
            m_eventLoop.quit();
        });
        connect(errorSender, errorSignal, this, [&]() {
            m_signalReceived = true;
            m_waitResult = WAIT_ERR_SIG;
            m_eventLoop.quit();
        });
        connect(this, &SignalWaiter::sigAbort, [&]() {
            m_signalReceived = true;
            m_waitResult = WAIT_ABORT;
            m_eventLoop.quit();
        });
        connect(&m_timeoutTimer, &QTimer::timeout, this, [&]() {
            m_waitResult = WAIT_TIMEOUT;
            m_eventLoop.quit();
        });
    };

    QEventLoop m_eventLoop;
    QTimer m_timeoutTimer;
    int m_timeoutMs = 0;
    WaitResult m_waitResult = WAIT_UNDEF;
    bool m_signalReceived = false;
    bool m_signalsConnected = false;
};

#endif // SIGNALWAITER_H
