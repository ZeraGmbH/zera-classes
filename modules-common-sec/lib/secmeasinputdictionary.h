#ifndef SECMEASINPUTDICTIONARY_H
#define SECMEASINPUTDICTIONARY_H

#include "secconfigdatacommon.h"
#include <QHash>
#include <QMap>

class SecMeasInputDictionary
{
public:
    void fillFromConfig(const TSecCommonInputConfigs &config);

    const QStringList &getInputNameList() const;
    QStringList getInputAliasList() const;

    void setAlias(const QString &inputName, const QString &alias);
    QString getAlias(const QString &inputName) const;

    QString getInputFNameFromAlias(const QString &alias) const;
private:
    QStringList m_inputsOrderedByConfig; // changing config sequence shall make it into GUI
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q" / "P AC" / "P DC"...
    QHash<QString /* inputName */, QString /* resource */> m_resourceHash;
    QMap<QString /* inputName */, QString /* alias */> m_aliasMap;
    QHash<QString /* alias */, QString /* inputName */> m_reverseAliasHash;
};

#endif // SECMEASINPUTDICTIONARY_H
