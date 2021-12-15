#include <gtest/gtest.h>
#include <QFile>
#include "supportedsources.h"

TEST(TEST_SUPPORTED_SOURCES, TYPE_FILENAMES_COMPLETE) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString jsonFileName = cSourceJsonFilenames::getJsonFileName(SupportedSourceTypes(type));
        EXPECT_EQ(jsonFileName.isEmpty(), false);
    }
}

TEST(TEST_SUPPORTED_SOURCES, STRUCTURE_PATHS_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString path = cSourceJsonFilenames::getJsonStructurePath(SupportedSourceTypes(type));
        QString jsonFileName = cSourceJsonFilenames::getJsonFileName(SupportedSourceTypes(type));
        EXPECT_EQ(path.contains(jsonFileName), true);
        EXPECT_EQ(QFile::exists(path), true);
    }
}

// overlaps TEST(TEST_DEVICEINFO, ALL_VALID) but focus on cSourceJsonStructureLoader
TEST(TEST_SUPPORTED_SOURCES, TYPES_LOAD_STRUCTURE_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QJsonObject structure = cSourceJsonStructureLoader::getJsonStructure(SupportedSourceTypes(type));
        EXPECT_EQ(structure.isEmpty(), false);
    }
}

