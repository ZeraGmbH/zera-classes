#ifndef MULTIREFERENCECONSTANTCONTAINER_H
#define MULTIREFERENCECONSTANTCONTAINER_H

#include <QObject>

class MultiReferenceConstantContainer : public QObject
{
    Q_OBJECT
public:
    MultiReferenceConstantContainer();
    void init(QStringList referenceInputNames);
    void catchInterfaceAnswer(quint32 msgnr, quint8 reply, QVariant answer);

signals:

};

#endif // MULTIREFERENCECONSTANTCONTAINER_H
