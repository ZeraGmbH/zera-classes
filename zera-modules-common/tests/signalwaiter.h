#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include <QObject>
#include <QEventLoop>
#include <QTimer>

class SignalWaiter : public QObject
{
    Q_OBJECT
public:
    enum WaitResult {
        WAIT_OK_SIG,
        WAIT_ERR_SIG,
        WAIT_ABORT,
        WAIT_TIMEOUT,
        WAIT_UNDEF
    };
    template <typename Func1>
    WaitResult WaitForSignal(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                       int timeout = 0)
    {
        return WaitForSignal(doneSender, doneSignal, this, &SignalWaiter::sigDefaultNoError, timeout);
    }
    template <typename Func1, typename Func2>
    WaitResult WaitForSignal(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                             const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal,
                             int timeout = 0)
    {
        WaitResult result = WAIT_UNDEF;
        connect(doneSender, doneSignal, [&]() {
            result = WAIT_OK_SIG;
            m_eventLoop.quit();
        });
        connect(errorSender, errorSignal, [&]() {
            result = WAIT_ERR_SIG;
            m_eventLoop.quit();
        });
        connect(this, &SignalWaiter::sigAbort, [&]() {
            result = WAIT_ABORT;
            m_eventLoop.quit();
        });
        QTimer timer;
        connect(&timer, &QTimer::timeout, this, [&]() {
            result = WAIT_TIMEOUT;
            m_eventLoop.quit();
        });
        if(timeout > 0) {
            timer.start(timeout);
        }
        m_eventLoop.exec();
        return result;
    };
    void abort() { emit sigAbort(); }
signals:
    void sigDefaultNoError();
    void sigAbort();
private:
    QEventLoop m_eventLoop;
};

#endif // SIGNALWAITER_H
