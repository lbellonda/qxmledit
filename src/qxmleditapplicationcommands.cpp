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

#include "qxmleditapplication.h"
#include "mainwindow.h"
#include "modules/encoding/codepagedialog.h"
#include "uiservices.h"
#include "visualization/datavisualization.h"
#include "modules/services/anotifier.h"
#include "StartParams.h"
#include "utils.h"
#include "qxmleditconfig.h"
#include "modules/anonymize/anonymizebatch.h"
#include "extraction/extractfragmentsdialog.h"
#include "modules/help/firstaccessdialog.h"
#include "modules/help/guidedoperationsdialog.h"

void QXmlEditApplication::onCommandNew()
{
    Utils::TODO_THIS_RELEASE("fare");
    MainWindow *mainWindow = new MainWindow(false, _appData);
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();
}


void QXmlEditApplication::onCommandQuit()
{
    Utils::TODO_THIS_RELEASE("fare");
    this->quit();
}

void QXmlEditApplication::onCommandOpen()
{
    Utils::TODO_THIS_RELEASE("fare");
    MainWindow *mainWindow = new MainWindow(false, _appData);
    Utils::TODO_THIS_RELEASE("fare ultimo path usato");
    if(mainWindow->openFileUsingDialog("XXXXXXX TODO", MainWindow::OpenUsingSameWindow)) {
        mainWindow->show();
        mainWindow->raise();
        mainWindow->activateWindow();
    } else {
        delete mainWindow;
    }
}

void QXmlEditApplication::onCommandValidate()
{
    showValidationOperationsPanel();
}

