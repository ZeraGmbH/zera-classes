#ifndef DEMORANGEDSPVALUES_H
#define DEMORANGEDSPVALUES_H

#include <QVector>

class DemoRangeDspValues
{
public:
    DemoRangeDspValues(int channels);
    void setValue(QVector<float> randomChannelRMS, double frequency);
    QVector<float> getDspValues();
private:
    int m_channels;
    QVector<float> m_demoRangeValues;
};

#endif // DEMORANGEDSPVALUES_H
