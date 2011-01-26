/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Components project on Qt Labs.
**
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions contained
** in the Technology Preview License Agreement accompanying this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
****************************************************************************/

#include "sdeclarativescreen.h"
#include <QtTest/QtTest>
#include <QUrl>
#include <QDeclarativeComponent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QSignalSpy>

class tst_SDeclarativeScreen : public QObject
{
    Q_OBJECT
private slots:

    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();
    void testScreenBasics();
    void testChangeOrientation();
    void testChangeScreenSize();

private:
    QDeclarativeView *window;
    QObject *screen;
};

void tst_SDeclarativeScreen::init()
{
}

void tst_SDeclarativeScreen::cleanup()
{
}

void tst_SDeclarativeScreen::initTestCase()
{
    window = new QDeclarativeView(QUrl::fromLocalFile("tst_declarativescreen.qml"));
    window->show();
    screen = qVariantValue<QObject *>(window->engine()->rootContext()->contextProperty("screen"));
}

void tst_SDeclarativeScreen::cleanupTestCase()
{
    delete window;
    window = 0;
}

void tst_SDeclarativeScreen::testScreenBasics()
{
    QTest::qWaitForWindowShown(window);
    QVERIFY(screen->property("width").toInt() > 0);
    QVERIFY(screen->property("height").toInt() > 0);
    QVERIFY(!screen->property("orientationLocked").toBool());

    if (screen->property("orientation").toInt() == SDeclarativeScreen::Portrait
        || (screen->property("orientation").toInt() == SDeclarativeScreen::PortraitInverted)) {
        QVERIFY(screen->property("width").toInt() < screen->property("height").toInt());
    } else if (screen->property("orientation").toInt() == SDeclarativeScreen::Landscape
        || screen->property("orientation").toInt() == SDeclarativeScreen::LandscapeInverted) {
        QVERIFY(screen->property("height").toInt() < screen->property("width").toInt());
    }
}

void tst_SDeclarativeScreen::testChangeOrientation()
{
    QSignalSpy spy(screen, SIGNAL(orientationChanged()));

    if (screen->property("orientation").toInt() == SDeclarativeScreen::Portrait
        || screen->property("orientation").toInt() == SDeclarativeScreen::PortraitInverted) {
        screen->setProperty("orientation", SDeclarativeScreen::Landscape);
        QVERIFY(screen->property("width").toInt() > screen->property("height").toInt());
        QVERIFY(screen->property("orientationString").toString() == QString("LandscapeInverted")
                || screen->property("orientationString").toString() == QString("Landscape"));
    } else {
        screen->setProperty("orientation", SDeclarativeScreen::Portrait);
        QVERIFY(screen->property("height").toInt() > screen->property("width").toInt());
        QVERIFY(screen->property("orientationString").toString() == QString("Portrait")
                || screen->property("orientationString").toString() == QString("PortraitInverted"));
    }
    // ensure that resizeEvent gets handled
    QApplication::sendPostedEvents();
    QCOMPARE(spy.count(), 1); // orientation changed once
}

void tst_SDeclarativeScreen::testChangeScreenSize()
{
    QMetaObject::invokeMethod(screen,"_q_updateScreenSize", Q_ARG(QSize, QSize(360,640)));
    screen->setProperty("orientation", SDeclarativeScreen::Portrait);
    QCOMPARE(screen->property("width").toInt(), 360);
    QCOMPARE(screen->property("height").toInt(), 640);
    QMetaObject::invokeMethod(screen, "_q_updateScreenSize", Q_ARG(QSize, QSize(640,360)));
    QCOMPARE(screen->property("width").toInt(), 640);
    QCOMPARE(screen->property("height").toInt(), 360);
}

QTEST_MAIN(tst_SDeclarativeScreen)

#include "tst_declarativescreen.moc"