#ifndef MEASMODESELECTOR_H
#define MEASMODESELECTOR_H

#include "measmode.h"
#include <QObject>
#include "QHash"

class MeasModeSelector : public QObject
{
    Q_OBJECT
public:
    bool addMode(std::shared_ptr<MeasMode> mode);
    void tryChangeMode(QString modeName);
    std::shared_ptr<MeasMode> getCurrMode() const;
    bool canChangeMask(QString mask) const;
    void tryChangeMask(QString mask);
    QString getCurrentMask() const;
    const QHash<QString, std::shared_ptr<MeasMode>> &getModes() const;
signals:
    void sigTransactionOk();
    void sigTransactionFailed();
private:
    QHash<QString, std::shared_ptr<MeasMode>> m_modes;
    std::shared_ptr<MeasMode> m_currentMode;
};

#endif // MEASMODESELECTOR_H
