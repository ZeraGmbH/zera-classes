#ifndef TASKGETINTERNALSOURCECAPABILITIES_H
#define TASKGETINTERNALSOURCECAPABILITIES_H

#include <pcbinterface.h>
#include <tasktemplate.h>
#include <QJsonObject>

class TaskGetInternalSourceCapabilities : public TaskTemplate
{
    Q_OBJECT
public:
    static TaskTemplatePtr create(Zera::PcbInterfacePtr pcbInterface,
                                  std::shared_ptr<QJsonObject> jsonCapabilities,
                                  int timeout, std::function<void()> additionalErrorHandler = []{});
    TaskGetInternalSourceCapabilities(Zera::PcbInterfacePtr pcbInterface,
                                      std::shared_ptr<QJsonObject> jsonCapabilities);
    void start() override;

private slots:
    void onServerAnswer(quint32 msgnr, quint8 reply, QVariant answer);
    void onServerError(QAbstractSocket::SocketError);
private:
    quint32 sendToServer();
    Zera::PcbInterfacePtr m_pcbInterface;
    std::shared_ptr<QJsonObject> m_jsonCapabilities;
    quint32 m_msgnr = 0;
};

#endif // TASKGETINTERNALSOURCECAPABILITIES_H
