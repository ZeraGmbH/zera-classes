#ifndef MODULEACITVIST_H
#define MODULEACITVIST_H

#include <QObject>

// pure virtual class for all objects living in a module, which generate an interface
// and/or which can do something after it got activated

class cModuleActivist: public QObject
{
    Q_OBJECT

public:
    cModuleActivist(){}
    virtual ~cModuleActivist(){}

signals:
    void activated();
    void deactivated();
    void activationError();

public slots:
    virtual void activate() = 0; // here we query our properties and activate ourself
    virtual void deactivate() = 0; // what do you think ? yes you're right

protected:
    virtual void generateInterface() = 0; // here we export our interface (entities)
    virtual void deleteInterface() = 0; // we delete interface in case of reconfiguration

};

#endif // MODULEACITVIST_H
