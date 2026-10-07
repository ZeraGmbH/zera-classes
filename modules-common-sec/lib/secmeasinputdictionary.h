#ifndef SECMEASINPUTDICTIONARY_H
#define SECMEASINPUTDICTIONARY_H

#include "secconfigdatacommon.h"
#include "secresourcetypelist.h"
#include <QHash>
#include <QMap>

class SecMeasInputDictionary
{
public:
    void fillFromReferenceConfig(const TSecCommonReferenceConfigs &refConfig);

    void addReferenceInput(const QString &inputName, const QString &resource);
    QString getResource(const QString &inputName) const;
    const QStringList &getResourceTypeList() const;

    QStringList getInputNameList() const;
    QStringList getInputAliasList() const;

    void setAlias(const QString &inputName, const QString &alias);
    QString getAlias(const QString &inputName) const;

    QString getInputFNameFromAlias(const QString &alias) const;
private:
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q" / "P AC" / "P DC"...
    QHash<QString /* inputName */, QString /* resource */> m_resourceHash;
    QMap<QString /* inputName */, QString /* alias */> m_aliasMap;
    QHash<QString /* alias */, QString /* inputName */> m_reverseAliasHash;

    SecResourceTypeList m_resourceTypeList;
};

#endif // SECMEASINPUTDICTIONARY_H
