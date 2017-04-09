/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

extern const char *APP_TITLE ;

#include <xmlEdit.h>
#include "utils.h"
#include "mainwindow.h"
#include "qxmleditconfig.h"
#include "modules/xml/xmlerrormanagerdialog.h"

/* Logic for ask for modifications (to be replicated in tests)
 * Operation: before:  1! n  after: 1! n
 * Reload              a  a         -  -
 * Drop                a  -         -  s
 * Session             a  -         -  s
 * Recent              a  -         -  s
 * Preferred           a  -         -  s
 * New Clip            a  -         -  s
 * New Spec            a  -         -  s
 * Load                a  -         -  s
 *
 * a: ask unconditionally.
 * s: ask if the file is in one of the editor windows.
 *
 * Ask:
 * -if single + mod -> ask
 * -if force + mod -> ask
 */

class MainWindowIOHelper
{
    bool otherWindow ;
    MainWindow *theWindow;
public:
    MainWindowIOHelper();
    ~MainWindowIOHelper();

    MainWindow *getWindow(MainWindow *baseWindow, const MainWindow::EWindowOpen useWindow, const QString &file = "");
    MainWindow *result(const bool ok);
};

MainWindowIOHelper::MainWindowIOHelper()
{
    theWindow = NULL  ;
    otherWindow = false ;
}

MainWindowIOHelper::~MainWindowIOHelper()
{

}

MainWindow *MainWindowIOHelper::getWindow(MainWindow *baseWindow, const MainWindow::EWindowOpen useWindow, const QString &filePath)
{
    const bool forceSameWindow = (MainWindow::OpenUsingSameWindow == useWindow);
    const bool forceNewWindow = (MainWindow::OpenUsingNewWindow == useWindow);
    theWindow = baseWindow ;
    otherWindow = false ;
    if(baseWindow->controller()->isOpenInNewWindow() && !forceSameWindow) {
        if(!baseWindow->getRegola()->isEmpty(false)) {
            theWindow = NULL ;
            // find an existing window;
            if(!filePath.isEmpty()) {
                theWindow = baseWindow->appData()->findWindowByPath(filePath);
                if(NULL != theWindow) {
                    if(!theWindow->checkAbandonChanges(MainWindow::OpenUsingNewWindow, filePath)) {
                        return NULL ;
                    }
                }
            }
            if(NULL == theWindow) {
                theWindow = baseWindow->makeNewWindow();
                otherWindow = true ;
            }
        }
    } else if(forceNewWindow) {
        theWindow = baseWindow->makeNewWindow();
        otherWindow = true ;
    }
    return theWindow;
}

MainWindow *MainWindowIOHelper::result(const bool ok)
{
    if(!ok) {
        if(otherWindow) {
            theWindow->close();
            theWindow->deleteLater();
        }
        theWindow = NULL ;
    }
    return theWindow ;
}

//---

bool MainWindow::loadFile(const QString &filePath, const bool activateModes, const EWindowOpen useWindow, const bool isRegularFile)
{
    MainWindow *result = loadFileAndReturnWindow(filePath, activateModes, useWindow, isRegularFile);
    if(NULL == result) {
        return false;
    }
    result->setVisible(true);
    return true;
}

MainWindow *MainWindow::loadFileAndReturnWindow(const QString &filePath, const bool activateModes,
        const EWindowOpen useWindow, const bool isRegularFile)
{
    beforeLoadingNewData();
    MainWindowIOHelper ioHelper;
    MainWindow *theWindow = ioHelper.getWindow(this, useWindow, filePath);
    if(NULL == theWindow) {
        return NULL ;
    }
    bool ok = theWindow->loadFileInner(filePath, isRegularFile, activateModes);
    theWindow->clearExportFilePath();
    return ioHelper.result(ok);
}

MainWindow *MainWindow::createFromClipboard(const EWindowOpen useWindow)
{
    beforeLoadingNewData();
    MainWindowIOHelper ioHelper;
    MainWindow *theWindow = ioHelper.getWindow(this, useWindow);
    bool ok = theWindow->newFromClipboard();
    return ioHelper.result(ok);
}

bool MainWindow::loadFileInner(const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    return loadFileInnerStream(filePath, isRegularFile, activateModes);
}

bool MainWindow::loadFileInnerDom(const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    bool fileLoaded = false;
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            QDomDocument document;
            QString errorMsg ;
            int errorLine = 0, errorColumn = 0;
            if(document.setContent(&file, &errorMsg, &errorLine, &errorColumn)) {
                if(isRegularFile) {
                    data->sessionManager()->enrollFile(filePath);
                }
                setDocument(document, filePath, true);
                if(isRegularFile) {
                    updateRecentFilesMenu(filePath);
                } else {
                    getRegola()->setFileName("");
                }
                updateWindowFilePath();
                autoLoadValidation();
                fileLoaded = true ;
                if(activateModes) {
                    activateModesOnNewFile();
                }
            } else {
                showLoadFileError(filePath, errorMsg, errorLine, errorColumn);
            }
            file.close();
        } else {
            errorOnLoad(file);
        }
    } else {
        errorFileName();
    }
    return fileLoaded;
}

bool MainWindow::loadFileInnerStream(QIODevice *ioDevice, const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    bool fileLoaded = false ;
    QXmlStreamReader reader ;
    reader.setDevice(ioDevice);
    XMLLoadStatus status;
    if(readData(&status, &reader, filePath, true, this)) {
        if(isRegularFile) {
            data->sessionManager()->enrollFile(filePath);
            updateRecentFilesMenu(filePath);
            if(status.areErrorsPresent()) {
                ui.loadWarningWidget->setVisible(true);
            }
        } else {
            getRegola()->setFileName("");
        }
        updateWindowFilePath();
        autoLoadValidation();
        fileLoaded = true ;
        if(activateModes) {
            activateModesOnNewFile();
        }
    }
    return fileLoaded;
}

bool MainWindow::loadFileInnerStream(const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    bool fileLoaded = false;
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            fileLoaded = loadFileInnerStream(&file, filePath, isRegularFile, activateModes);
            file.close();
        } else {
            errorOnLoad(file);
        }
    } else {
        errorFileName();
    }
    return fileLoaded;
}

void MainWindow::errorOnLoad(QFile &file)
{
    Utils::error(this, QString(tr("Unable to read data.\nError code is '%1', %2.")).arg(file.error()).arg(file.errorString()));
}

void MainWindow::errorFileName()
{
    Utils::error(this, tr("File name empty.\nUnable to load."));
}

#ifdef QXMLEDIT_TEST
void MainWindow::setLoadErrorHandler(XMLLoadErrorHandler *newHandler)
{
    _loadErrorHandler = newHandler ;
}
#endif

bool MainWindow::showErrorAndAskUserIfContinue(QWidget * /*parent*/, XMLLoadContext *context, QXmlStreamReader *xmlReader)
{
#ifdef QXMLEDIT_TEST
    if(NULL != _loadErrorHandler) {
        return _loadErrorHandler->showErrorAndAskUserIfContinue(this, context, xmlReader);
    }
    return false;
#endif
    XMLErrorManagerDialog errorManager(this, context, xmlReader);
    if(errorManager.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}
