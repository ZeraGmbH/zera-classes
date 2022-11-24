#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include <QObject>
#include <QEventLoop>
#include <QTimer>

/**
 * @brief A class for blocked wait on signals. It enables structured code for
 * simple sequences e.g as an alternate using QStateMachines which creates
 * uneasy to read and unflexible code
 */
class SignalWaiter : public QObject
{
    Q_OBJECT
public:
    enum WaitResult { WAIT_OK_SIG, WAIT_ERR_SIG, WAIT_ABORT, WAIT_TIMEOUT, WAIT_UNDEF };

    template <typename Func1>
    SignalWaiter(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                 int timeout = 0) {
        setup(doneSender, doneSignal, this, &SignalWaiter::sigDefaultNoError, timeout);
    }
    template <typename Func1, typename Func2>
    SignalWaiter(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                 const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal,
                 int timeout = 0) {
        setup(doneSender, doneSignal, errorSender, errorSignal, timeout);
    }

    bool wait() {
        if(!m_signalReceived) {
            if(m_timeoutMs > 0) {
                m_timeoutTimer.start(m_timeoutMs);
            }
            m_eventLoop.exec();
        }
        return m_waitResult == WAIT_OK_SIG;
    }
    WaitResult getResult() {
        return m_waitResult;
    }
    void abort() { emit sigAbort(); }
signals:
    void sigDefaultNoError();
    void sigAbort();
private:
    template <typename Func1, typename Func2>
    void setup(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                             const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal,
                             int timeout = 0)
    {
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
};

#endif // SIGNALWAITER_H
