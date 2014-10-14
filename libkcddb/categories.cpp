// Copyright (C) 2005 by Shaheed Haque (srhaque@iee.org). All rights reserved.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//

#include "categories.h"

#include "kcddbi18n.h"

class KCDDB::Categories::Private
{
public:
  QStringList cddb;
  QStringList i18n;
};

KCDDB::Categories::Categories()
  : d(new Private)
{
    // These are only 11 Category values defined by CDDB. See
    //
    // http://www.freedb.org/modules.php?name=Sections&sop=viewarticle&artid=26
    //
    d->cddb << QLatin1String( "blues" ) << QLatin1String( "classical" ) << QLatin1String( "country" ) <<
        QLatin1String( "data" ) << QLatin1String( "folk" ) << QLatin1String( "jazz" ) << QLatin1String( "misc" ) <<
        QLatin1String( "newage" ) << QLatin1String( "reggae" ) << QLatin1String( "rock" ) << QLatin1String( "soundtrack" );
    d->i18n << i18n("Blues") << i18n("Classical") << i18nc("music genre", "Country") <<
            i18n("Data") << i18n("Folk") << i18n("Jazz") << i18n("Miscellaneous") <<
            i18n("New Age") << i18n("Reggae") << i18n("Rock") << i18n("Soundtrack");
}

KCDDB::Categories::~Categories()
{
    delete d;
}

KCDDB::Categories::Categories(const Categories& other)
    :d(new Private)
{
    *d = *other.d;
}

KCDDB::Categories& KCDDB::Categories::operator=(const Categories& other)
{
    *d = *other.d;
    return *this;
}

const QStringList &KCDDB::Categories::cddbList() const
{
    return d->cddb;
}

const QStringList &KCDDB::Categories::i18nList() const
{
    return d->i18n;
}

const QString KCDDB::Categories::cddb2i18n(const QString &category) const
{
    int index = d->cddb.indexOf(category.trimmed());
    if (index != -1)
    {
        return d->i18n[index];
    }
    else
    {
        return cddb2i18n(QLatin1String( "misc" ));
    }
}

const QString KCDDB::Categories::i18n2cddb(const QString &category) const
{
    int index = d->i18n.indexOf(category.trimmed());
    if (index != -1)
    {
        return d->cddb[index];
    }
    else
    {
        return QLatin1String( "misc" );
    }
}
