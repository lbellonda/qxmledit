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
#include "modules/xml/configuregeneralindentation.h"
#include "modules/style/editingtypesdialog.h"
#include "base64dialog.h"
#include "compare.h"

void extractFragments(ExtractResults *extractResult, QWidget *parent, QWidget *mainWidget);

void QXmlEditApplication::onCommandNew()
{
    MainWindow *mainWindow = new MainWindow(false, _appData);
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();
}


void QXmlEditApplication::onCommandQuit()
{
    this->quit();
}

void QXmlEditApplication::onCommandOpen()
{
    MainWindow *mainWindow = new MainWindow(false, _appData);
    if(mainWindow->openFileUsingDialog(QXmlEditData::sysFilePathForOperation(""), MainWindow::OpenUsingSameWindow)) {
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


void QXmlEditApplication::onCommandFormatting()
{
    ConfigureGeneralIndentation configureGeneralIndentation(_guidedOperationsDialog, _appData);
    configureGeneralIndentation.setModal(true);
    configureGeneralIndentation.setWindowModality(Qt::ApplicationModal);
    configureGeneralIndentation.exec();
}

void QXmlEditApplication::onCommandConfigureVision()
{
    taskChooseDetail();
}

void QXmlEditApplication::onCommandConfigureEditing()
{
    Utils::TODO_THIS_RELEASE("si apre ma il radiobutton non e' selezionato");
    EditingTypesDialog editingTypesDialog(_appData, _guidedOperationsDialog);
    editingTypesDialog.setWindowModality(Qt::ApplicationModal);
    editingTypesDialog.exec();
}

void QXmlEditApplication::onCommandConfigure()
{
    _appData->updateEditors(_appData->preferences(_guidedOperationsDialog));
}

void QXmlEditApplication::onCommandUserProfile()
{
    if(NULL != _guidedOperationsDialog) {
        _appData->setUserGuidedOperation(!_appData->isUserGuidedOperation());
        _guidedOperationsDialog->applyUserProfile();
    }
}

void QXmlEditApplication::onCommandExtractFile()
{
    ExtractResults results;
    extractFragments(&results, _guidedOperationsDialog, _guidedOperationsDialog);
}

void QXmlEditApplication::onCommandViewXSD()
{
    Utils::TODO_THIS_RELEASE("fare");
    MainWindow *mainWindow = getOrCreateMainWindow();
    if(NULL != mainWindow) {
        if(mainWindow->openFileUsingDialog(QXmlEditData::sysFilePathForOperation(""), MainWindow::OpenUsingSameWindow)) {
            if(mainWindow->isValidXsd()) {
                mainWindow->viewAsXSD();
            }
        }
    }
}

void QXmlEditApplication::onCommandUserManual()
{
    _appData->showUserManual();
}

void QXmlEditApplication::onCommandCompare()
{
    Regola dummy;
    CompareBridge::doCompare(_guidedOperationsDialog, false, &dummy, uiDelegate(), _appData->lastFiles());
}

void QXmlEditApplication::onCommandAnon()
{
    _appData->anonymizeFile(_guidedOperationsDialog);
}

void QXmlEditApplication::onCommandEditingShortcut()
{
    _appData->showEditingShortcuts(_guidedOperationsDialog);
}

void QXmlEditApplication::onCommandBase64()
{
    Utils::TODO_THIS_RELEASE("test");
    _appData->uiServices()->doBase64Dialog();
}

void QXmlEditApplication::onCommandViewXMLMap()
{
    Utils::TODO_THIS_RELEASE("controlla codice");
    Utils::TODO_THIS_RELEASE("raggruppare il load dei file");
    QString fileName = Utils::askFileNameToOpen(_guidedOperationsDialog, QXmlEditData::sysFilePathForOperation(""));
    DataVisualization::viewData(_appData, _guidedOperationsDialog, _guidedOperationsDialog, fileName);
}
