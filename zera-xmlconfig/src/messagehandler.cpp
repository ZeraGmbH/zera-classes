#include "messagehandler.h"

MessageHandler::MessageHandler()
    : QAbstractMessageHandler(0)
{
}


QString MessageHandler::statusMessage()
{
    return m_description;
}


int MessageHandler::line()
{
    return m_sourceLocation.line();
}


int MessageHandler::column()
{
    return m_sourceLocation.column();
}


void MessageHandler::handleMessage(QtMsgType type, const QString &description,
                               const QUrl &identifier, const QSourceLocation &sourceLocation)
{
    Q_UNUSED(type);
    Q_UNUSED(identifier);

    m_messageType = type;
    m_description = description;
    m_sourceLocation = sourceLocation;
}
