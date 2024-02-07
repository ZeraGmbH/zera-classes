#ifndef ABSTRACTACTUALVALUEGENERATOR_H
#define ABSTRACTACTUALVALUEGENERATOR_H

#include <QObject>
#include <memory>

class AbstractActualValueGenerator : public QObject
{
    Q_OBJECT
public:
    virtual void start() = 0;
    virtual void stop() = 0;
signals:
    void sigNewActualValues(QVector<float> demoValues);
};

typedef std::unique_ptr<AbstractActualValueGenerator> AbstractActualValueGeneratorPtr;

#endif // ABSTRACTACTUALVALUEGENERATOR_H
