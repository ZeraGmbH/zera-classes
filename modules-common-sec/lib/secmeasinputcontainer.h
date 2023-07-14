#ifndef SECMEASINPUTCONTAINER_H
#define SECMEASINPUTCONTAINER_H

#include <QHash>

class SecMeasInputContainer
{
public:
    SecMeasInputContainer();
    void addReferenceInput(QString inputName, QString resource);
    QString getResource(QString inputName) const;
    QStringList getInputNameList();

    void setAlias(QString inputName, QString alias);
    QString getAlias(QString inputName) const;

    void setDisplayedString(QString inputName, QString displayName);
    QString getInputNameFromDisplayedName(QString displayName) const;

    void setNotificationId(QString inputName, int notificationId);
    QString getInputNameFromNotificationId(int notificationId);

    void setCurrentInput(QString currentRefIn);
    QString getCurrentInput() const;
private:
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q"...
    // displayed "P AC" / "P DC"...
    QHash<QString /* refInputName */, QString /* resource */> m_resourceHash;
    QHash<QString /* refInputName */, QString /* alias */> m_aliasHash;
    QHash<QString /* displayName */, QString /* refInputName */> m_displayHash;
    QHash<int /* notId */, QString /* refInputName */> m_notificationIdHash;
    QString m_currentInput;
};

#endif // SECMEASINPUTCONTAINER_H