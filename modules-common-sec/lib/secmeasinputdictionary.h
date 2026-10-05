#ifndef SECMEASINPUTDICTIONARY_H
#define SECMEASINPUTDICTIONARY_H

#include <QHash>
#include <QMap>

class SecMeasInputDictionary
{
public:
    void addReferenceInput(const QString &inputFName, const QString &resource);
    QString getResource(const QString &inputFName) const;

    QStringList getInputNameList() const;
    QStringList getInputAliasList() const;

    void setAlias(const QString &inputFName, const QString &alias);
    QString getAlias(const QString &inputFName) const;

    void setNotificationId(const QString &inputFName, int notificationId);
    QString getInputNameFromNotificationId(int notificationId) const;

    QString getInputFNameFromAlias(const QString &alias) const;
private:
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q" / "P AC" / "P DC"...
    QHash<QString /* inputFName */, QString /* resource */> m_resourceHash;
    QMap<QString /* inputFName */, QString /* alias */> m_aliasMap;
    QHash<QString /* alias */, QString /* inputFName */> m_reverseAliasHash;
    QMap<int /* notifyId */, QString /* refPowerName */> m_notificationIdMap;
};

#endif // SECMEASINPUTDICTIONARY_H
