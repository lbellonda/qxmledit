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
#include "config.h"

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
    if(baseWindow->controller()->isOpenInNewWidow() && !forceSameWindow) {
        if(!baseWindow->getRegola()->isEmpty(false)) {
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
    return true;
}

MainWindow *MainWindow::loadFileAndReturnWindow(const QString &filePath, const bool activateModes,
        const EWindowOpen useWindow, const bool isRegularFile)
{
    MainWindowIOHelper ioHelper;
    MainWindow *theWindow = ioHelper.getWindow(this, useWindow, filePath);
    if(NULL == theWindow) {
        return NULL ;
    }
    bool ok = theWindow->loadFileInner(filePath, isRegularFile, activateModes);
    return ioHelper.result(ok);
}

MainWindow *MainWindow::createFromClipboard(const EWindowOpen useWindow)
{
    MainWindowIOHelper ioHelper;
    MainWindow *theWindow = ioHelper.getWindow(this, useWindow);
    bool ok = theWindow->newFromClipboard();
    return ioHelper.result(ok);
}

bool MainWindow::loadFileInner(const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    if(Config::getBool(Config::KEY_XML_LOAD_STREAM, true)) {
        return loadFileInnerStream(filePath, isRegularFile, activateModes);
    } else {
        return loadFileInnerDom(filePath, isRegularFile, activateModes);
    }
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
                    if(Utils::fileIsXSLT(getEditor()->getRegola())) {
                        activateXSLTonNewFile();
                    }
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

bool MainWindow::loadFileInnerStream(const QString &filePath, const bool isRegularFile, const bool activateModes)
{
    bool fileLoaded = false;
    if(!filePath.isEmpty()) {
        QFile file(filePath);
        if(file.open(QIODevice::ReadOnly)) {
            QXmlStreamReader reader ;
            reader.setDevice(&file);
            if(readData(&reader, filePath, true)) {
                if(isRegularFile) {
                    data->sessionManager()->enrollFile(filePath);
                    updateRecentFilesMenu(filePath);
                } else {
                    getRegola()->setFileName("");
                }
                updateWindowFilePath();
                autoLoadValidation();
                fileLoaded = true ;
                if(activateModes) {
                    if(Utils::fileIsXSLT(getEditor()->getRegola())) {
                        activateXSLTonNewFile();
                    }
                }
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

void MainWindow::errorOnLoad(QFile &file)
{
    Utils::error(this, QString(tr("Unable to read data.\nError code is '%1'")).arg(file.error()));
}

void MainWindow::errorFileName()
{
    Utils::error(this, tr("File name empty.\nUnable to load."));
}
