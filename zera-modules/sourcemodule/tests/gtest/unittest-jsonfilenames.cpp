#include <gtest/gtest.h>
#include <QFile>
#include "supportedsources.h"
#include "jsonfilenames.h"

TEST(TEST_JSON_FILENAMES, TYPE_FILENAMES_COMPLETE) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString jsonFileName = JsonFilenames::getJsonFileName(SupportedSourceTypes(type));
        EXPECT_EQ(jsonFileName.isEmpty(), false);
    }
}

TEST(TEST_JSON_FILENAMES, STRUCTURE_PATHS_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString path = JsonFilenames::getJsonStructurePath(SupportedSourceTypes(type));
        QString jsonFileName = JsonFilenames::getJsonFileName(SupportedSourceTypes(type));
        EXPECT_EQ(path.contains(jsonFileName), true);
        EXPECT_EQ(QFile::exists(path), true);
    }
}

