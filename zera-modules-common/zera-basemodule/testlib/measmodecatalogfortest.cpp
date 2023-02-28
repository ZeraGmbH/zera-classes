#include "measmodecatalogfortest.h"

void MeasModeCatalogForTest::enableTest()
{
    m_warningHandler = [](QString name) {
        Q_UNUSED(name)
    };}
