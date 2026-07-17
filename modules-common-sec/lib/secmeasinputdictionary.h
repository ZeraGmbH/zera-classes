#ifndef SECMEASINPUTDICTIONARY_H
#define SECMEASINPUTDICTIONARY_H

#include <QHash>
#include <QMap>

class SecMeasInputDictionary
{
public:
    void addReferenceInput(const QString &inputName, const QString &resource);
    QString getResource(const QString &inputName) const;
    QStringList getInputNameList();

    void setAlias(const QString &inputName, const QString &alias);
    QString getAlias(const QString &inputName) const;

    void setDisplayedString(const QString &inputName, const QString &displayName);
    QString getInputNameFromDisplayedName(const QString &displayName) const;

    void setNotificationId(const QString &inputName, int notificationId);
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
