#ifndef TEST_ADJ_MODULE_GAIN_H
#define TEST_ADJ_MODULE_GAIN_H

#include <QObject>

class test_adj_module_gain : public QObject
{
    Q_OBJECT
private slots:
    void noActValuesWithPermission();
    void validActValuesWithPermission();
    void validActValuesWithoutPermission();

    // from here on with permission
    void outOfLimitLower();
    void outOfLimitLowerInverted();
    void outOfLimitUpper();
    void outOfLimitUpperInverted();
    void denyRangeNotSet();

    void oneNodeWithinLimitLower();
    void oneNodeWithinLimitLowerInverted();
    void oneNodeWithinLimitUpper();
    void oneNodeWithinLimitUpperInverted();

    void twoNodesCheckNodesGenerated();
    void twoNodesCheckNodesGeneratedInverted();

    // Test on correction values should go zenux-services
    void oneNodeCheckCorrectionsCalculated();
    void oneNodeCheckCorrectionsCalculatedInverted();
    void twoNodesCheckCorrectionsCalculatedLinear();
    void twoNodesCheckCorrectionsCalculatedLinearInverted();
    void twoNodesCheckCorrectionsCalculatedNonLinear();
    void twoNodesCheckCorrectionsCalculatedNonLinearInverted();
};

#endif // TEST_ADJ_MODULE_GAIN_H
