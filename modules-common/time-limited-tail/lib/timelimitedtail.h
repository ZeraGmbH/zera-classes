#ifndef TIMELIMITEDTAIL_H
#define TIMELIMITEDTAIL_H

#include <QVector>
#include <QList>
#include <QObject>

class TimeLimitedTail : public QObject
{
    Q_OBJECT
public:
    explicit TimeLimitedTail(int tailLengthMs = 1000);
    void setTailLength(int tailLengthMs);

    void appendValues(const QVector<float> &values);
    const QList<QVector<float>> &getValues() const;
signals:
    void sigContentsChanged();

private:
    QList<QVector<float>> m_values;
};

#endif // TIMELIMITEDTAIL_H
