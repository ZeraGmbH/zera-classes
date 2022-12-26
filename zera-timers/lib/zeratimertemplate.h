#ifndef ZERATIMERTEMPLATE_H
#define ZERATIMERTEMPLATE_H

#include <QObject>
#include <memory>

class ZeraTimerTemplate : public QObject
{
    Q_OBJECT
public:
    explicit ZeraTimerTemplate(int expireTimeMs);
    virtual ~ZeraTimerTemplate() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
protected:
    int m_expireTimeMs;
signals:
    void sigExpired();
};

typedef std::unique_ptr<ZeraTimerTemplate> ZeraTimerTemplatePtr;

#endif // ZERATIMERTEMPLATE_H
