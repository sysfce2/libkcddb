/*
  Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2002 Benjamin Meyer <ben-devel@meyerhome.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlayout.h>

#include "cddbconfigwidget.h"

CDDBConfigWidget::CDDBConfigWidget(QWidget * parent, const char * name)
  : CDDBConfigWidgetBase(parent, name)
{
  // Connections from widgets to slotConfigChanged are made in designer.

  if (0 == layout())
  {
    qDebug("No layout. Designer bug. Hacking it.");

    QVBoxLayout * layout = new QVBoxLayout(this, 0, 6);

    layout->addWidget(GroupBox4);
    layout->addWidget(GroupBox3);
    layout->addWidget(GroupBox1);
    layout->addWidget(cacheEnable);
    layout->addStretch(1);
  }
}

  void
CDDBConfigWidget::slotConfigChanged()
{
  emit(configChanged());
}

// vim:tabstop=2:shiftwidth=2:expandtab:cinoptions=(s,U1,m1
