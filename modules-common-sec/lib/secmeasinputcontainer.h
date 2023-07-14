#ifndef SECMEASINPUTCONTAINER_H
#define SECMEASINPUTCONTAINER_H

#include <QHash>

class SecMeasInputContainer
{
public:
    SecMeasInputContainer();
    void addReferenceInput(QString inputName, QString resource);
    void setAlias(QString inputName, QString alias);
    void setDisplayedString(QString inputName, QString displayName);
    QString getResource(QString inputName) const;
    QString getAlias(QString inputName) const;
    QString getInputNameFromDisplayedName(QString displayName) const;
    QStringList getInputNameList();

    void setCurrentInput(QString currentRefIn);
    QString getCurrentInput() const;
private:
    // input name: "f0" / "f1"... or DUT ec0
    // alias "P" / "Q"...
    // displayed "P AC" / "P DC"...
    QHash<QString /* refInputName */, QString /* resource */> m_resourceHash;
    QHash<QString /* refInputName */, QString /* alias */> m_aliasHash;
    QHash<QString /* displayName */, QString /* refInputName */> m_displayHash;
    QString m_currentInput;
};

#endif // SECMEASINPUTCONTAINER_H
