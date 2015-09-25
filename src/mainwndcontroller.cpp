/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#include "mainwndcontroller.h"
#include "mainwindow.h"
#include "utils.h"
#include "config.h"
#include "infodialog.h"
#include "modules/anonymize/anonymizedialog.h"
#include "modules/anonymize/anonallalg.h"
#include "modules/anonymize/anoncodealg.h"
#include "modules/anonymize/anonseqproducer.h"
#include "modules/anonymize/anonfixedalg.h"
#include "modules/anonymize/anoncontext.h"
#include "modules/anonymize/anonadvdialog.h"
#include "modules/anonymize/anonimyzebatchdialog.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/xsd/xsiinsertattribute.h"

MainWndController::MainWndController(QObject *parent) :
    QObject(parent)
{
    _w = NULL ;
}

MainWndController::~MainWndController()
{
}

void MainWndController::setWindow(MainWindow *theWindow)
{
    _w = theWindow ;
}

void MainWndController::anonymize(AnonContext *context)
{
    if(_w->isReadOnly()) {
        return ;
    }
    if(NULL == _w->getRegola()->root()) {
        return ;
    }
    innerAnonymize(context);
}

void MainWndController::anonymize(AnonAlg *alg)
{
    if(_w->isReadOnly()) {
        return ;
    }
    if(NULL == _w->getRegola()->root()) {
        return ;
    }
    innerAnonymize(alg);
}

void MainWndController::onAnonymize()
{
    if(_w->isReadOnly()) {
        return ;
    }
    if(NULL == _w->getRegola()->root()) {
        return ;
    }
    Utils::warning(_w, tr("This feature will modify your data."));
    AnonAdvDialog dlg(_w->appData(), _w->getRegola(), _w);
    if(dlg.exec() == QDialog::Accepted) {
        AnonContext *ctx = dlg.getContext();
        anonymize(ctx);
        delete ctx;
    }
}

void MainWndController::onAnonymizeFile()
{
    AnonimyzeBatchDialog dlg(_w->appData(), _w);
    dlg.setModal(true);
    dlg.exec();
}

bool MainWndController::anonymizeGetParams(AnonymizeParameters *params)
{
    AnonymizeDialog dlg(_w, params);
    if(dlg.exec() == QDialog::Accepted) {
        return true ;
    }
    return false ;
}

void MainWndController::innerAnonymize(AnonContext *context)
{
    if(_w->isReadOnly()) {
        return ;
    }
    Regola *regola = _w->getRegola();
    regola->anonymize(context, _w->getEditor()->getMainTreeWidget());
}

void MainWndController::innerAnonymize(AnonAlg *alg)
{
    if(_w->isReadOnly()) {
        return ;
    }
    AnonContext context(NULL, QString(""));
    context.setAlg(alg);
    anonymize(&context);
}


QString MainWndController::askNewXSIType(Element *selection)
{
    if(NULL == selection) {
        return "" ;
    }
    // get the namespace
    QString nsNsd = _w->appData()->namespaceManager()->namespaceUri(NamespaceManager::XSD_NAMESPACE);
    QString prefixNS ;
    selection->findPrefixForNamespace(nsNsd, prefixNS);
    return askNewXSITypeAttribute(_w, prefixNS);
}

bool MainWndController::isOpenInNewWidow()
{
    return Config::getBool(Config::KEY_GENERAL_OPEN_NEWWINDOW, QXmlEditData::DefaultOpenInNewWindow);
}

bool MainWndController::actionInfo()
{
    Regola *regola = _w->getRegola();
    if(NULL != regola) {
        InfoDialog infoDialog(_w, regola, _w->isReadOnly());
        if(infoDialog.exec() == QDialog::Accepted) {
            if(!_w->isReadOnly()) {
                return regola->setNewDTD(infoDialog.dtd());
            }
        }
    }
    return false;
}

// takes care of activating snippet in this or in a new window
void MainWndController::createDocumentFromSnippet(Regola* newRegola)
{
    MainWindow *theWindow = _w ;
    if(isOpenInNewWidow()) {
        theWindow = _w->makeNewWindow();
    }
    if(!theWindow->checkAbandonChanges()) {
        return ;
    }
    theWindow->createDocumentFromSnippet(newRegola);
}


bool MainWndController::createDocumentFromResources(const QString &path)
{
    if(!_w->checkAbandonChanges()) {
        return false ;
    }
    MainWindow *window = _w->loadFileAndReturnWindow(path, true, MainWindow::OpenUsingDefaultSettings, false);
    if(NULL != window) {
        window->markAsAllEdited();
        return true;
    }
    return false;
}
