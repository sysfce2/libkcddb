/*
  Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2002 Benjamin Meyer <ben-devel@meyerhome.net>
  CopyRight (C) 2002 Nadeem Hasan <nhasan@kde.org>

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

#include <qregexp.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kstringhandler.h>

#include "cddb.h"

namespace KCDDB
{
  CDDB::CDDB()
    : user_( "libkcddb-user" ),
      localHostName_( "localHost" ),
      readOnly_( false )
  {
    socket_.setTimeout( 60 );
  }

  CDDB::~CDDB()
  {
    // Empty.
  }

    QString
  CDDB::trackOffsetListToId()
  {
    return trackOffsetListToId( trackOffsetList_ );
  }
    QString
  CDDB::trackOffsetListToId( const TrackOffsetList & list )
  {
    // Taken from version by Michael Matz in kio_audiocd.
    unsigned int id = 0;
    int numTracks = list.count() - 2;

    // The last two in the list are disc begin and disc end.
    for ( int i = numTracks-1; i >= 0; i-- )
    {
      int n = list[ i ]/75;
      while ( n > 0 )
      {
        id += n % 10;
        n /= 10;
      }
    }

    unsigned int l = (list[numTracks + 1] + 1) / 75;
    l -= list[numTracks] / 75;

    id = ( ( id % 255 ) << 24 ) | ( l << 8 ) | numTracks;

    return QString::number( id, 16 ).rightJustify( 8, '0' );
  }

    QString
  CDDB::trackOffsetListToString()
  {
    QString ret;
    uint numTracks = trackOffsetList_.count()-2;

    // Disc start.
    ret.append( QString::number( numTracks ) );
    ret.append( " " );

    for ( uint i = 0; i < numTracks; i++ )
    {
      ret.append( QString::number( trackOffsetList_[ i ] ) );
      ret.append( " " );
    }

    unsigned int discLengthInSec = ( trackOffsetList_[ numTracks+1 ] ) / 75;

    // Disc length in seconds.
    ret.append( QString::number( discLengthInSec ) );

    return ret;
  }

    bool
  CDDB::parseGreeting( const QString & line )
  {
    uint serverStatus = statusCode( line );

    if ( 200 == serverStatus )
    {
      kdDebug() << "Server response: read-only" << endl;
      readOnly_ = true;
    }
    else if ( 201 == serverStatus )
    {
      kdDebug() << "Server response: read-write" << endl;
    }
    else
    {
      kdDebug() << "Server response: bugger off" << endl;
      return false;
    }

    return true;
  }

    bool
  CDDB::parseHandshake( const QString & line )
  {
    uint serverStatus = statusCode( line );

    if ( ( 200 != serverStatus ) && ( 402 != serverStatus ) )
    {
      kdDebug() << "Handshake was too tight. Letting go." << endl;
      return false;
    }

    kdDebug() << "Handshake was warm and firm" << endl;

    return true;
  }

    QString
  CDDB::readLine()
  {
    if ( !isConnected() )
    {
      kdDebug() << "socket status: " << socket_.socketStatus() << endl;
      return QString::null;
    }

    const uint maxRead = 4096;

    QByteArray buf( maxRead );
    Q_LONG bytesRead = socket_.readLine( buf.data(), maxRead - 1 );
    buf[ bytesRead ] = '\0';

    return QString( buf );
  }

    void
  CDDB::writeLine( const QString & line )
  {
    if ( !isConnected() )
    {
      kdDebug() << "socket status: " << socket_.socketStatus() << endl;
      return;
    }

    kdDebug() << "WRITE: [" << line << "]" << endl;
    QCString buf = line.latin1();
    buf.append( "\n" );

    socket_.writeBlock( buf.data(), buf.length() );
  }

    uint
  CDDB::statusCode( const QString & line )
  {
    QStringList tokenList = QStringList::split( ' ', line );

    uint serverStatus = tokenList[ 0 ].toUInt();

    return serverStatus;
  }

    CDDB::Transport
  CDDB::stringToTransport(const QString & s)
  {
    if  ("HTTP" == s )
      return HTTP;
    else if ( "CDDB" == s )
      return CDDBP;
    else
      return SMTP;
  }

    QString
  CDDB::resultToString(Result r)
  {
    switch (r)
    {
      case Success:
        return "Success";
        break;

      case HostNotFound:
        return "HostNotFound";
        break;

      case NoResponse:
        return "NoResponse";
        break;

      case NoRecordFound:
        return "NoRecordFound";
        break;

      case MultipleRecordFound:
        return "MultipleRecordFound";
        break;

      case CannotSave:
        return "CannotSave";
        break;

      default:
        return "UnknownError";
        break;
    }
  }

    QString
  CDDB::transportToString(uint t)
  {
    switch (Transport(t))
    {
      case HTTP:
        return "HTTP";
        break;

      case CDDBP:
        return "CDDBP";
        break;

      case SMTP:
        return "SMTP";
        break;

      default:
        return "UnknownTransport";
        break;
    }
  }
}

// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
