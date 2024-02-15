#ifndef ABSTRACTACTVALMANINTHEMIDDLE_H
#define ABSTRACTACTVALMANINTHEMIDDLE_H

#include <QObject>
#include <memory>

class AbstractActValManInTheMiddle : public QObject
{
    Q_OBJECT
public:
    virtual void start() = 0;
    virtual void stop() = 0;
public slots:
    virtual void onNewActualValues(QVector<float>* values) = 0; // the modules want us so...
signals:
    void sigNewActualValues(QVector<float>* values);
};

typedef std::shared_ptr<AbstractActValManInTheMiddle> AbstractActValManInTheMiddlePtr;

#endif // ABSTRACTACTVALMANINTHEMIDDLE_H
