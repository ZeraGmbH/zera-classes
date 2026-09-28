#ifndef TEST_UINT32EXPANDER_H
#define TEST_UINT32EXPANDER_H

#include <QObject>

class test_uint32expander : public QObject
{
    Q_OBJECT
private slots:
    void initZero();
    void setValueOnce();
    void setValueTwiceWithoutOverflow();
    void setValueTwiceWithoutOverflowEqual();
    void setValueTwiceWithOverflow();
    void setValueMultipleOverflow();

    void setValueTwiceWithOverflowThenReset();

    void doubleLimits();
};

#endif // TEST_UINT32EXPANDER_H
