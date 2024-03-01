#ifndef TEST_DEMOVALUESDSPRMS_H
#define TEST_DEMOVALUESDSPRMS_H

#include <QObject>

class test_demovaluesdsprms : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
    void addCurrent();
    void setSymmetric();
private:
    static const QStringList mtRmsLayout;
};

#endif
