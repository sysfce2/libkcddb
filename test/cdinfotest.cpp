/*
  Copyright (C) 2005 Richard Lärkäng <nouseforaname@home.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include <QtTest/qtest.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kdebug.h>
#include "libkcddb/cdinfo.h"
#include "cdinfotest.h"

using namespace KCDDB;

void CDInfoTest::testLongLines()
{
    CDInfo info;

    QString str;

    TrackInfo& tinfo = info.track(0);

    for (int i=0; i <= 100; i++)
    {
        str.fill('A',10*i);
        info.set(Title, str);
        str.fill('B',10*i);
        info.set(Comment, str);
        str.fill('C',10*i);
        info.set(Artist, str);
        str.fill('D',10*i);
        info.set(Genre, str);
        str.fill('E',10*i);
        info.set(Length, str);
        str.fill('F',10*i);
        info.set("custom", str);
        str.fill('G',10*i);
        tinfo.set(Title, str);
        QString data = info.toString();

        CDInfo info2;
        info2.load(data);
        str.fill('A',10*i);
        QCOMPARE(info2.get(Title).toString(), str);
        str.fill('B',10*i);
        QCOMPARE(info2.get(Comment).toString(), str);
        str.fill('C',10*i);
        QCOMPARE(info2.get(Artist).toString(), str);
        str.fill('D',10*i);
        if (i == 0)
            QCOMPARE(info2.get(Genre).toString(), QString("Unknown"));
        else
            QCOMPARE(info2.get(Genre).toString(), str);
        str.fill('E',10*i);
        QCOMPARE(info2.get(Length).toString(), str);
        str.fill('F',10*i);
        QCOMPARE(info2.get("custom").toString(), str);
        str.fill('G',10*i);
        QCOMPARE(info2.track(0).get(Title).toString(), str);
    }
}

int main(int argc, char *argv[])
{
    setenv("LC_ALL", "C", 1);
    KAboutData aboutData( "qttest", "qttest", "version" );
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication::disableAutoDcopRegistration();
    KApplication app(false);
    CDInfoTest tc;
    return QTest::qExec( &tc, argc, argv );
}

#include "cdinfotest.moc"