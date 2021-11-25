#include <gtest/gtest.h>
#include <QJsonArray>
#include "sourcedevicestatus.h"

using SOURCEMODULE::cSourceDeviceStatus;

static const QString keyBusy = "busy";
static const QString keyErrors = "errors";
static const QString keyWarnings = "warnings";

TEST(TEST_SOURCEDEVICESTATUS, INIT) {
    cSourceDeviceStatus status;
    QJsonObject json = status.getJsonStatus();

    // Just in case we change: Our clients have to be modified either
    EXPECT_TRUE(json.contains(keyBusy));
    EXPECT_TRUE(json.contains(keyErrors));
    EXPECT_TRUE(json.contains(keyWarnings));

    EXPECT_TRUE(json[keyBusy].isBool());
    EXPECT_FALSE(json[keyBusy].toBool());

    EXPECT_TRUE(json[keyErrors].isArray());
    EXPECT_EQ(json[keyErrors].toArray().count(), 0);

    EXPECT_TRUE(json[keyWarnings].isArray());
    EXPECT_EQ(json[keyWarnings].toArray().count(), 0);
}

TEST(TEST_SOURCEDEVICESTATUS, BUSY) {
    cSourceDeviceStatus status;
    QJsonObject json = status.getJsonStatus();
    int origCount = json.count();
    EXPECT_FALSE(status.getBusy());
    status.setBusy(true);
    EXPECT_TRUE(status.getBusy());
    // key typo test
    json = status.getJsonStatus();
    int newCount = json.count();
    EXPECT_EQ(origCount, newCount);
}

TEST(TEST_SOURCEDEVICESTATUS, WARNINGS) {
    cSourceDeviceStatus status;
    QJsonObject json = status.getJsonStatus();
    int origCount = json.count();
    EXPECT_EQ(status.getWarnings().count(), 0);
    status.addWarning("foo");
    EXPECT_EQ(status.getWarnings().count(), 1);
    EXPECT_EQ(status.getWarnings()[0], "foo");
    // key typo test
    json = status.getJsonStatus();
    int newCount = json.count();
    EXPECT_EQ(origCount, newCount);
}

TEST(TEST_SOURCEDEVICESTATUS, ERRORS) {
    cSourceDeviceStatus status;
    QJsonObject json = status.getJsonStatus();
    int origCount = json.count();
    EXPECT_EQ(status.getErrors().count(), 0);
    status.addError("foo");
    EXPECT_EQ(status.getErrors().count(), 1);
    EXPECT_EQ(status.getErrors()[0], "foo");
    // key typo test
    json = status.getJsonStatus();
    int newCount = json.count();
    EXPECT_EQ(origCount, newCount);
}
