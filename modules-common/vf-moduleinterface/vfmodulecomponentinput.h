#ifndef VEINMODULECOMPONENTINPUT
#define VEINMODULECOMPONENTINPUT

#include <QString>
#include <QVariant>

class VfModuleComponentInput: public QObject
{
    Q_OBJECT

public:
    VfModuleComponentInput(int entityid, QString name);
    const int m_nEntityId;
    const QString m_sName;
    QVariant value() const;
    void setValue(const QVariant &value);
signals:
    void sigValueChanged(QVariant);
private:
    friend class VfEventSystemInputComponents;
    QVariant m_value;
};

#endif // VEINMODULECOMPONENTINPUT

