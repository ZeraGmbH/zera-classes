#ifndef TEST_DEMORMSDSPVALUES_H
#define TEST_DEMORMSDSPVALUES_H

#include <QObject>

class test_demormsdspvalues : public QObject
{
    Q_OBJECT
private slots:
    void addVoltage();
    void setSymmetric();
private:
    static const QStringList mtRmsLayout;
};

#endif
