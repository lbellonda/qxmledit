/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2018 by Luca Bellonda and individual contributors       *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/

#include "testhelp.h"
#include "mainwindow.h"
#include "modules/help/searchcommanddialog.h"

TestHelp::TestHelp()
{
}

TestHelp::~TestHelp()
{
}

bool TestHelp::testFast()
{
    return testUnit();
}
bool TestHelp::testUnit()
{
    _testName = "testUnit";
    if(!testSearchCommands()) {
        return false;
    }
    return true;
}

bool TestHelp::errorSelAction(const QString &type)
{
    return error(QString("Failed for action: %1").arg(type));
}

bool TestHelp::testSearchCommands()
{
    _subTestName = "testSearchCommands";
    {
        QList<QAction*> list;
        SearchCommandDialog dlg(list);
        QAction enabledAction(NULL);
        enabledAction.setEnabled(true);
        if(!dlg.filterAction(&enabledAction)) {
            return errorSelAction("enabled");
        }
    }
    {
        QList<QAction*> list;
        SearchCommandDialog dlg(list);
        QAction disabledAction(NULL);
        disabledAction.setEnabled(false);
        if(dlg.filterAction(&disabledAction)) {
            return errorSelAction("disabled");
        }
    }
    {
        QList<QAction*> list;
        SearchCommandDialog dlg(list);
        QAction hiddenAction(NULL);
        hiddenAction.setEnabled(true);
        hiddenAction.setVisible(false);
        if(dlg.filterAction(&hiddenAction)) {
            return errorSelAction("hidden");
        }
    }
    {
        QList<QAction*> list;
        SearchCommandDialog dlg(list);
        QAction taggedAction(NULL);
        taggedAction.setEnabled(true);
        taggedAction.setVisible(true);
        taggedAction.setData(MainWindow::ActionTagLastFiles);
        if(dlg.filterAction(&taggedAction)) {
            return errorSelAction("tagged");
        }
    }
    return true ;
}
