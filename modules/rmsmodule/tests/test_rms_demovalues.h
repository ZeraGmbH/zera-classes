#ifndef TEST_RMSDEMOVALUES_H
#define TEST_RMSDEMOVALUES_H

#include <QObject>

class test_rms_demovalues : public QObject
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
