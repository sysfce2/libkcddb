/*
  Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2002 Benjamin Meyer <ben-devel@meyerhome.net>
  Copyright (C) 2002 Nadeem Hasan <nhasan@kde.org>

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
  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <kstandarddirs.h>

#include <qfile.h>
#include <qdir.h>

#include "cache.h"

namespace KCDDB
{
    QString
  Cache::fileName( const CDInfo& info )
  {
    QString cddbCacheDir = locateLocal( "cache", "cddb/" );

    QDir dir( cddbCacheDir );
    if ( !dir.exists( info.genre ) )
      dir.mkdir( info.genre );

    QString cacheFile = cddbCacheDir + info.genre + "/" + info.id;

    return cacheFile;
  }

    CDInfoList
  Cache::lookup( const QString &cddbId )
  {
    kdDebug() << "Looking up " << cddbId << " in CDDB cache" << endl;

    CDInfoList infoList;
    QString cddbCacheDir = locateLocal( "cache", "cddb/" );

    QDir dir( cddbCacheDir );
    QStringList dirList = dir.entryList( QDir::Dirs );

    QStringList::ConstIterator it = dirList.begin();

    while ( it != dirList.end() )
    {
      QString genre( *it );
      if ( genre[ 0 ] != '.' )
      {
        QFile f( cddbCacheDir + genre + "/" + cddbId );
        if (f.exists())
        {
          if ( f.open(IO_ReadOnly) )
          {
            QTextStream ts(&f);
            QString cddbData = ts.read();
            f.close();
            CDInfo info;
            info.load(cddbData);
            info.genre = genre;
            infoList.append( info );
          }
        }
      }
      ++it;
    }

    return infoList;
  }

    void
  Cache::store(const CDInfoList& list)
  {
    CDInfoList::ConstIterator it=list.begin(); 
    while (it!=list.end())
    {
      CDInfo info( *it );
      QString cacheFile = fileName(info);
      kdDebug() << "Storing " << cacheFile << " in CDDB cache" << endl;

      QFile f(cacheFile);
      if ( f.open(IO_WriteOnly) )
      {
        QTextStream ts(&f);
        ts << info.toString();
        f.close();
      }

      ++it;
    }
  }
}

// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1