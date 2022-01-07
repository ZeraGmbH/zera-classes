#ifndef SOURCEIDKEEPER_H
#define SOURCEIDKEEPER_H

class SourceIdKeeper
{
public:
    SourceIdKeeper();

    void setCurrent(int id);
    int getCurrent();

    bool isCurrAndDeactivateIf(int id);

    bool isActive();
    void deactivate();
private:
    bool m_isActive = false;
    int m_currentId;
};

#endif // SOURCEIDKEEPER_H
