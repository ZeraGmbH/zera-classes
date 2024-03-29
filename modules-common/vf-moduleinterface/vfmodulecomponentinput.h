#ifndef VEINMODULECOMPONENTINPUT
#define VEINMODULECOMPONENTINPUT

#include <QString>
#include <QVariant>

class VfModuleComponentInput: public QObject
{
    Q_OBJECT

public:
    VfModuleComponentInput(int entityid, QString name) :
        m_nEntityId(entityid),
        m_sName(name) { }
    int m_nEntityId;
    QString m_sName;
    QVariant value() const;
    void setValue(const QVariant &value);
private:
    QVariant m_value;
signals:
    void sigValueChanged(QVariant);
};

#endif // VEINMODULECOMPONENTINPUT

