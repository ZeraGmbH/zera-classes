#ifndef IOQUERYCONTENTEVALUATOR_H
#define IOQUERYCONTENTEVALUATOR_H

#include <QByteArray>
#include <QSharedPointer>

class IIoQueryContentEvaluator
{
public:
    typedef QSharedPointer<IIoQueryContentEvaluator> Ptr;
    virtual bool evalQueryResponse(QByteArray queryContent) = 0;
};

class IoQueryContentEvaluatorAlwaysPass : public IIoQueryContentEvaluator
{
public:
    bool evalQueryResponse(QByteArray) override { return true; }
};

class IoQueryContentEvaluatorAlwaysFail : public IIoQueryContentEvaluator
{
public:
    bool evalQueryResponse(QByteArray) override { return false; }
};

#endif // IOQUERYCONTENTEVALUATOR_H
