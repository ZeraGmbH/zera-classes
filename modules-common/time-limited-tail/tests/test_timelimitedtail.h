#ifndef TEST_TIMELIMITEDTAIL_H
#define TEST_TIMELIMITEDTAIL_H

#include <QObject>

class test_timelimitedtail : public QObject
{
    Q_OBJECT
private slots:
    void initialEmpty();
    void appendOneSizeOne();
    void appendTwoSizeTwo();
};

#endif // TEST_TIMELIMITEDTAIL_H
