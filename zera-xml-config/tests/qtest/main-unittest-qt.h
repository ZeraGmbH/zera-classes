#ifndef MAIN_UNITTEST_H
#define MAIN_UNITTEST_H

#include <QObject>
#include <QList>
#include <QString>
#include <QtTest>

QObject* addTest(QObject* test);

static constexpr int shortQtEventTimeout = 5;

#endif // MAIN_UNITTEST_H
