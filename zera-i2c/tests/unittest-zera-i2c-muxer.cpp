#include "i2cmuxerscopedonoff.h"
#include "i2cmuxernull.h"
#include <gtest/gtest.h>

class I2cMuxerTest : public I2cMuxerInterface
{
public:
    void enableMuxChannel() override { m_enableCount++; }
    void disableMux() override { m_enableCount--; }
    int getEnableCount() { return m_enableCount; }
private:
    int m_enableCount = 0;
};


TEST(TEST_I2CMUX_SCOPED_ON_OFF, I2cMuxerTest) {
    I2cMuxerTest *i2cMuxerTest = new I2cMuxerTest();
    I2cMuxerInterface::Ptr i2cMuxer = I2cMuxerInterface::Ptr(i2cMuxerTest);

    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
    i2cMuxer->enableMuxChannel();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    i2cMuxer->enableMuxChannel();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 2);
    i2cMuxer->disableMux();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    i2cMuxer->disableMux();
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
}

TEST(TEST_I2CMUX_SCOPED_ON_OFF, I2cMuxerScopedOnOff) {
    I2cMuxerTest *i2cMuxerTest = new I2cMuxerTest();
    I2cMuxerInterface::Ptr i2cMuxer = I2cMuxerInterface::Ptr(i2cMuxerTest);
    I2cMuxerScopedOnOff *i2cMuxerScoped = new I2cMuxerScopedOnOff(i2cMuxer);

    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 1);
    delete i2cMuxerScoped;
    EXPECT_EQ(i2cMuxerTest->getEnableCount(), 0);
}
