#ifndef SIGNALWAITER_H
#define SIGNALWAITER_H

#include <QObject>
#include <QEventLoop>

class SignalWaiter : public QObject
{
    Q_OBJECT
signals:
    void defaultNoError(QString);
public:
    template <typename Func1>
    bool WaitForSignal(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal, int)
    {
        return WaitForSignal(doneSender, doneSignal, this, &SignalWaiter::defaultNoError);
    }
    template <typename Func1, typename Func2>
    bool WaitForSignal(const typename QtPrivate::FunctionPointer<Func1>::Object *doneSender, Func1 doneSignal,
                       const typename QtPrivate::FunctionPointer<Func2>::Object *errorSender, Func2 errorSignal)
    {
        m_error.clear();
        bool signalReceived = false;
        connect(doneSender, doneSignal, [&] () {
            signalReceived = true;
            m_eventLoop.quit();
        });
        connect(errorSender, errorSignal, [&] (QString error) {
            m_error = error;
            m_eventLoop.quit();
        });
        m_eventLoop.exec();
        return signalReceived;
    };
    void abort() { m_eventLoop.quit(); }
    QString getError() { return m_error; }
private:
    QEventLoop m_eventLoop;
    QString m_error;
};

#endif // SIGNALWAITER_H
