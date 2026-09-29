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

    // |                act                                  |
    // |                final                                |
    // act == final => same upper
    void actEqualFinalNoOverflow();
    void actEqualFinalWithOverflow();

    // |                act                                  |
    // |                     final                           |
    // act < final / no overflow => same upper
    void actLessThanFinalNoOverflow();
    void actLessThanFinalWithOverflow();

    // |                       act                           |
    // |                final                                |
    // act > final / no overflow => same upper
    void actLargerThanFinalNoOverflow();
    void actLargerThanFinalWithOverflow();

    // |                                                 act |
    // | final                                               |
    // act < final / overflow => upper final == upper act + 1
    void actLessThanOverflowFinal();

    // | act                                                 |
    // |                                               final |
    // act > final / overflow => upper final == upper act - 1
    void actLargerThanOverflowFinal();


    // corners...
    void setActOnlyNoOverflow();
    void setActFinalActNoOverflow();
    void setFinalActFinalNoOverflow();

    void setActOnlyWithOverflow();
    void setActFinalWithOverflow();
    void setFinalActWithOverflow();
    void setActFinalActWithOverflow1();
    void setActFinalActWithOverflow2();
    void setFinalActFinalwithOverflow();
    void setActualSmallerFinalOverflowActual();

    void setActOnlyWithOverflowImmediateRead();
    void setActFinalWithOverflowImmediateRead();
    void setFinalActWithOverflowImmediateRead();
    void setActFinalActWithOverflow1ImmediateRead();
    void setActFinalActWithOverflow2ImmediateRead();
    void setFinalActFinalwithOverflowImmediateRead();
};

#endif // TEST_UINT32EXPANDER_H
