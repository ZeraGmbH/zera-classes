#include <gtest/gtest.h>
#include <QFile>
#include <zera-json-params-structure.h>
#include "supportedsources.h"
#include "jsonstructureloader.h"

TEST(TEST_JSON_STRUCTURE_LOADER, TYPES_ALL_STRUCTURES_LOADED) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        EXPECT_FALSE(structure.isEmpty());
    }
}

TEST(TEST_JSON_STRUCTURE_LOADER, TYPES_ALL_STRUCTURES_VALID) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        ZeraJsonParamsStructure paramStruct;
        EXPECT_TRUE(paramStruct.setJson(structure).isEmpty());
    }
}

TEST(TEST_JSON_STRUCTURE_LOADER, TYPES_LOAD_DEFAULT_DEMO) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QJsonObject structure = JsonStructureLoader::loadJsonDefaultStructure(SupportedSourceTypes(type));
        EXPECT_TRUE(structure["Name"].toString().startsWith("Demo"));
        EXPECT_EQ(structure["Version"].toString(), "---");
    }
}

TEST(TEST_JSON_STRUCTURE_LOADER, TYPES_SET_NAME) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString testName = "TestName";
        QJsonObject structure = JsonStructureLoader::loadJsonStructure(SupportedSourceTypes(type), testName, "");
        EXPECT_EQ(structure["Name"].toString(), testName);
    }
}

TEST(TEST_JSON_STRUCTURE_LOADER, TYPES_SET_VERSION) {
    for(int type=0; type<SOURCE_TYPE_COUNT; type++) {
        QString testVersion = "TestVersion";
        QJsonObject structure = JsonStructureLoader::loadJsonStructure(SupportedSourceTypes(type), "", testVersion);
        EXPECT_EQ(structure["Version"].toString(), testVersion);
    }
}

