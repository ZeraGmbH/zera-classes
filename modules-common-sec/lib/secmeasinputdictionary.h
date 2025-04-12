#ifndef SECMEASINPUTDICTIONARY_H
#define SECMEASINPUTDICTIONARY_H

#include <QHash>
#include <QMap>

class SecMeasInputDictionary
{
public:
    void addReferenceInput(QString inputName, QString resource);
    QString getResource(QString inputName) const;
    QStringList getInputNameList();

    void setAlias(QString inputName, QString alias);
    QString getAlias(QString inputName) const;

    void setDisplayedString(QString inputName, QString displayName);
    QString getInputNameFromDisplayedName(QString displayName) const;

    void setNotificationId(QString inputName, int notificationId);
    QString getInputNameFromNotificationId(int notificationId);
private:
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q"...
    // displayed "P AC" / "P DC"...
    QHash<QString /* refPowerName */, QString /* resource */> m_resourceHash;
    QHash<QString /* refPowerName */, QString /* alias */> m_aliasHash;
    QHash<QString /* displayName */, QString /* refPowerName */> m_displayHash;
    QMap<int /* notifyId */, QString /* refPowerName */> m_notificationIdHash;
};

#endif // SECMEASINPUTDICTIONARY_H
