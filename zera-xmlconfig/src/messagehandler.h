#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QAbstractMessageHandler>

class MessageHandler : public QAbstractMessageHandler
{
public:
    MessageHandler();
    QString statusMessage();
    int line();
    int column();

protected:
    virtual void handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation);

private:
    QtMsgType m_messageType;
    QString m_description;
    QSourceLocation m_sourceLocation;
};

#endif // MESSAGEHANDLER_H
