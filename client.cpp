/*
  Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2002 Benjamin Meyer <ben-devel@meyerhome.net>
  Copyright (C) 2003 Richard L�rk�ng <nouseforaname@home.se>

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

#include "client.h"
#include "synccddbplookup.h"
#include "asynccddbplookup.h"
#include "synchttplookup.h"
#include "asynchttplookup.h"
#include "syncsmtpsubmit.h"
#include "asyncsmtpsubmit.h"
#include "cache.h"
#include "lookup.h"

#include <kdebug.h>
#include <qsocket.h>

namespace KCDDB
{
  class Client::Private
  {
    public:

      Private()
        : block( true )
      {}

      Config config;
      CDInfoList cdInfoList;
      bool block;
  };

  Client::Client() : QObject()
  {
    d = new Private;
    d->config.readConfig();
  }

  Client::Client(const Config & config) : QObject()
  {
    d = new Private;
    d->config = config;
  }

  Client::~Client()
  {
    delete d;
  }

    const Config &
  Client::config() const
  {
    return d->config;
  }

    void
  Client::setBlockingMode( bool enable )
  {
    d->block = enable;
  }

    bool
  Client::blockingMode() const
  {
    return d->block;
  }

    CDInfoList
  Client::lookupResponse() const
  {
    return d->cdInfoList;
  }

    CDDB::Result
  Client::lookup(const TrackOffsetList & trackOffsetList)
  {
    d->cdInfoList.clear();

    QString cddbId = Lookup::trackOffsetListToId( trackOffsetList );

    if ( cddbId.isNull() )
    {
      kdDebug(60010) << "Can't create cddbid from offset list" << endl;
      return Lookup::NoRecordFound;
    }

    if ( Cache::Ignore != d->config.cachePolicy() )
    {
      d->cdInfoList = Cache::lookup( cddbId );

      kdDebug(60010) << "Found " << d->cdInfoList.count() << " hit(s)" << endl;

      if ( !d->cdInfoList.isEmpty() )
      {
        if ( !blockingMode() )
          emit finished( Lookup::Success );

        return CDDB::Success;
      }
    }

    if ( Cache::Only == d->config.cachePolicy() )
    {
      kdDebug(60010) << "Only trying cache. Give up now." << endl;
      return CDDB::NoRecordFound;
    }

    CDDB::Result r;
    Lookup::Transport t = ( Lookup::Transport )d->config.lookupTransport();

    if ( blockingMode() )
    {
      if( Lookup::CDDBP == t )
        cdInfoLookup = new SyncCDDBPLookup();
      else
        cdInfoLookup = new SyncHTTPLookup();

      r = cdInfoLookup->lookup( d->config.hostname(), 
              d->config.port(), trackOffsetList );

      if ( CDDB::Success == r )
      {
        d->cdInfoList = cdInfoLookup->lookupResponse();
        Cache::store( d->cdInfoList );
      }

      delete cdInfoLookup;
    }
    else
    {
      if( Lookup::CDDBP == t )
      {
        cdInfoLookup = new AsyncCDDBPLookup();

        connect( static_cast<AsyncCDDBPLookup *>( cdInfoLookup ), 
                  SIGNAL( finished( CDDB::Result ) ),
                  SLOT( slotFinished( CDDB::Result ) ) );
      }
      else
      {
        cdInfoLookup = new AsyncHTTPLookup();

        connect( static_cast<AsyncHTTPLookup *>( cdInfoLookup ), 
                  SIGNAL( finished( CDDB::Result ) ),
                  SLOT( slotFinished( CDDB::Result ) ) );
      }

      r = cdInfoLookup->lookup( d->config.hostname(), 
              d->config.port(), trackOffsetList );

      if ( Lookup::Success != r )
        delete cdInfoLookup;
    }

    return r;
  }

    void
  Client::slotFinished( CDDB::Result r )
  {
    if ( CDDB::Success == r )
    {
      d->cdInfoList = cdInfoLookup->lookupResponse();
      Cache::store( d->cdInfoList );
    }
    else
      d->cdInfoList.clear();

    emit finished( r );

    delete cdInfoLookup;
  }

    void
  Client::slotSubmitFinished( CDDB::Result r )
  {
    emit finished( r );

    delete cdInfoSubmit;
  }

    CDDB::Result
  Client::submit(const CDInfo &cdInfo, const TrackOffsetList& offsetList)
  {
    // Check if it's valid

    if(cdInfo.id == "0")
      return CDDB::CannotSave;

    uint last=0;
    for (uint i=0; i < (offsetList.count()-2); i++)
    {
      if(last >= offsetList[i])
        return CDDB::CannotSave;
      last = offsetList[i];
    }

    //TODO Check that it is edited

    switch (d->config.submitTransport())
    {
      case Submit::HTTP:
        // TODO For now...
        kdDebug(60010) << k_funcinfo << "HTTP Submit not supported yet: " << endl;
//          << CDDB::transportToString(d->config.submitTransport()) << endl;
        return CDDB::UnknownError;
        break;
      
      case Submit::SMTP:
      {
	QString hostname = d->config.smtpHostname();
	uint port = d->config.smtpPort();
	QString username = d->config.smtpUsername();
	QString from = d->config.emailAddress();
	if ( blockingMode() )
	  cdInfoSubmit = new SyncSMTPSubmit( hostname, port, username, from );
	else
	{
	  cdInfoSubmit = new AsyncSMTPSubmit( hostname, port, username, from );
          connect( static_cast<AsyncSMTPSubmit *>( cdInfoSubmit ), 
                  SIGNAL( finished( CDDB::Result ) ),
                  SLOT( slotSubmitFinished( CDDB::Result ) ) );
	}
        break;
      }
      case Submit::None:
        kdDebug() << k_funcinfo << "CDDB Submit disabled" << endl;
	return CDDB::UnknownError;
	break;
      default:
        kdDebug(60010) << k_funcinfo << "Unsupported transport: " << endl;
//          << CDDB::transportToString(d->config.submitTransport()) << endl;
        return CDDB::UnknownError;
        break;
    }

    CDDB::Result r = cdInfoSubmit->submit( cdInfo, offsetList );

    if ( blockingMode() )
      delete cdInfoSubmit;

    return r;
  }
}

#include "client.moc"


// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
