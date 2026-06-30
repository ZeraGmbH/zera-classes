#ifndef DEMODSPVALUESTHD_H
#define DEMODSPVALUESTHD_H

#include <QStringList>

class DemoDspValuesThd
{
public:
    explicit DemoDspValuesThd(const QStringList &valueChannelList);
    explicit DemoDspValuesThd(int entityId,
                               const QStringList &valueChannelList,
                               std::function<double(int)> valueGenerator);

    void setAllValuesSymmetric();
    const QVector<float> &getDspValues() const;

private:
    int m_entityId = 0;
    QStringList m_valueChannelList;
    std::function<double(int)> m_valueGenerator;

    QVector<float> m_dspValues;
};

#endif // DEMODSPVALUESTHD_H
