/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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
#include "qxmleditconfig.h"
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
#include "modules/replica/replicacloneinfo.h"
#include "modules/replica/replicaclonedialog.h"
#include "modules/replica/replicasettingsdialog.h"
#include "undo/prefixremovecommand.h"
#include "modules/namespace/removeprefixdialog.h"
#include "modules/namespace/removenamespacesdialog.h"
#include "modules/namespace/setprefixdialog.h"
#include "modules/namespace/setnamespacedialog.h"
#include "modules/namespace/replacenamespacedialog.h"
#include "modules/namespace/replaceprefixdialog.h"
#include "modules/namespace/normalizenamespacedialog.h"
#include "modules/messages/sourceerror.h"
#include "sourcemessagemanager.h"
#include "modules/messages/sourcerelatedmessages.h"
#if QT_VERSION >= QT_VERSION_CHECK(5,7,0)
#include <QtScxml/QScxmlStateMachine>
#endif
//----------
ReplicaInfoProvider::ReplicaInfoProvider() {}
ReplicaInfoProvider::~ReplicaInfoProvider() {}
//----------

MainWndController::MainWndController(QObject *parent) :
    QObject(parent)
{
    _w = NULL ;
    _replicaInfoProvider = this ;
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

bool MainWndController::isOpenInNewWindow()
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
    if(isOpenInNewWindow()) {
        theWindow = _w->makeNewWindow();
    }
    if(!theWindow->checkAbandonChanges()) {
        return ;
    }
    theWindow->createDocumentFromSnippet(newRegola);
    theWindow->clearExportFilePath();
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

bool MainWndController::cloneReplica()
{
    if(_w->isReadOnly()) {
        return false;
    }
    Element *element = _w->getSelectedItem();
    if(NULL != element) {
        ReplicaCloneInfo *info = _replicaInfoProvider->getCloneInfo(_w, element);
        if(NULL != info) {
            return _w->getEditor()->doReplica(info, element);
        }
    }
    return false;
}

ReplicaInfoProvider *MainWndController::setReplicaInfoProvider(ReplicaInfoProvider *theProvider)
{
    ReplicaInfoProvider *current = _replicaInfoProvider;
    _replicaInfoProvider = theProvider ;
    return current ;
}

ReplicaCloneInfo * MainWndController::getCloneInfo(QWidget *parent, Element *element)
{
    ReplicaCloneInfo *command = NULL ;
    ReplicaCloneDialog dlg(parent, element);
    if(dlg.exec() == QDialog::Accepted) {
        command = dlg.results();
        if(NULL != command) {
            if(command->numClones() > 100) {
                if(!Utils::askYN(parent, tr("You will create a very big number of objects (%1). Do you want to continue?").arg(command->numClones()))) {
                    delete command ;
                    command = NULL ;
                }
            }
        }
    }
    return command ;
}


void MainWndController::removePrefix()
{
    if(!_w->isReadOnly()) {
        RemovePrefixInfo info;
        info.element = _w->getSelectedItem();
        _w->getRegola()->collectPrefixes(info.prefixInfo, info.element);
        if(RemovePrefixDialog::getRemovePrefixInfo(_w, &info)) {
            _w->getEditor()->prefixRemove(info.removedPrefix, info.element, info.targetSelection, info.isAllPrefixes);
        }
    }
}

void MainWndController::removeNamespace()
{
    if(!_w->isReadOnly()) {
        RemoveNamespacesInfo info;
        info.element = _w->getSelectedItem();
        _w->getRegola()->collectNamespaces(info.namespacesInfo, info.element);
        if(RemoveNamespacesDialog::getRemoveNamespacesInfo(_w, &info)) {
            _w->getEditor()->namespaceRemove(info.removedNamespace, info.element, info.targetSelection, info.isAllNamespaces, info.isRemoveDeclarations);
        }
    }
}

void MainWndController::setPrefix()
{
    if(!_w->isReadOnly()) {
        SetPrefixInfo info;
        info.element = _w->getSelectedItem();
        _w->getRegola()->collectPrefixes(info.prefixInfo, info.element);
        if(SetPrefixDialog::getSetPrefixInfo(_w, &info)) {
            _w->getEditor()->prefixSet(info.setPrefix, info.element, info.targetSelection);
        }
    }
}

void MainWndController::setNamespace()
{
    if(!_w->isReadOnly()) {
        SetNamespaceInfo info;
        info.element = _w->getSelectedItem();
        _w->getRegola()->collectNamespaces(info.namespacesInfo, info.element);
        if(SetNamespaceDialog::getSetNamespaceInfo(_w, &info, _w->getRegola()->namespaceManager())) {
            _w->getEditor()->namespaceSet(info.nsURI, info.setPrefix, info.element, info.targetSelection, info.remapPrefixes, &info.namespacesInfo);
        }
    }
}

void MainWndController::replacePrefix()
{
    if(!_w->isReadOnly()) {
        ReplacePrefixInfo info;
        info.element = _w->getSelectedItem();
        _w->getRegola()->collectPrefixes(info.prefixInfo, info.element);
        if(ReplacePrefixDialog::getReplacePrefixInfo(_w, &info)) {
            _w->getEditor()->prefixReplace(info.toReplacePrefix, info.setPrefix, info.element, info.targetSelection, info.isAllPrefixes);
        }
    }
}

void MainWndController::replaceNamespace()
{
    if(!_w->isReadOnly()) {
        ReplaceNamespaceInfo info;
        info.element = _w->getSelectedItem();
        Regola *regola = _w->getRegola();
        regola->collectNamespaces(info.namespacesInfo, info.element);
        if(ReplaceNamespaceDialog::getReplaceNamespaceInfo(_w, &info, regola->namespaceManager())) {
            _w->getEditor()->namespaceReplace(info.toReplaceNSUri, info.nsURI, info.setPrefix, info.element, info.targetSelection, info.remapPrefixes, &info.namespacesInfo);
        }
    }
}

void MainWndController::normalizeNamespace()
{
    if(!_w->isReadOnly()) {
        NormalizeNamespaceInfo info;
        info.element = _w->getSelectedItem();
        Regola *regola = _w->getRegola();
        regola->collectNamespaces(info.namespacesInfo, info.element);
        if(NormalizeNamespaceDialog::getNormalizeNamespaceInfo(_w, &info, regola->namespaceManager())) {
            _w->getEditor()->namespaceNormalize(info.nsURI, info.setPrefix, info.element, info.targetSelection, info.declareOnRoot, info.remapPrefixes, &info.namespacesInfo);
        }
    }
}

QBuffer* MainWndController::getDataForSourceDecode()
{
    QBuffer *dataStream = new QBuffer();
    if(!_w->getRegola()->writeStream(dataStream, false)) {
        delete dataStream;
        return NULL;
    }
    dataStream->close();
    return dataStream;
}

void MainWndController::sourceDecode(QBuffer *dataStream, QList<SourceMessage*> &errors, Regola *regola)
{
    SourceMessageManager manager;
    manager.bindToSource(dataStream, errors, regola);
}

bool MainWndController::checkSCXML()
{
#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
    Utils::error(_w, tr("SCXML test is supported only starting from Qt 5.7.0"));
    return false;
#else
    Regola *regola = _w->getRegola();
    QBuffer *dataStream = getDataForSourceDecode();
    if(NULL == dataStream) {
        Utils::error(_w, tr("Unable to check the data as SCXML."));
        return false;
    }
    dataStream->open(QIODevice::ReadOnly);
    QScxmlStateMachine *testMachine = QScxmlStateMachine::fromData(dataStream);
    if(NULL == testMachine) {
        Utils::error(_w, tr("Unable to check the data as SCXML."));
        delete dataStream ;
        return false;
    }
    bool returnValue = false;
    if(testMachine->parseErrors().isEmpty()) {
        Utils::message(_w, tr("SCXML is valid."));
        returnValue = true;
    } else {
        const QString DataModel("datamodel");
        if((NULL != regola) && (NULL != regola->root())) {
            const QString &value = regola->root()->getAttributeValue(DataModel);
            if(!value.isEmpty() && ("null" != value) && ("ecmascript" != value) && !value.startsWith("cplusplus:")) {
                Utils::error(_w, tr("Currently only null, ecmascript or cpp datamodels are supported by the validator."));
            }
        }
        QString msg;
        QList<SourceMessage*> errors;
        int index = 1;
        bool doneMsg = false;
        bool visible = true ;
        foreach(const QScxmlError &error, testMachine->parseErrors()) {
            if(index > 3) {
                if(!doneMsg) {
                    msg += "More errors exist...\n";
                    visible = false;
                    doneMsg = true;
                }
            }
            SourceMessage *sourceError = new SourceMessage();
            sourceError->setColumn(error.column());
            sourceError->setLine(error.line());
            sourceError->setDescription(error.description());
            sourceError->setType(SourceMessage::Error);
            sourceError->setSource("SCXML");
            errors.append(sourceError);
            if(visible) {
                msg += QString("%1 - %2\n").arg(index).arg(error.description());
            }
            index ++;
        }
        Utils::error(_w, tr("Invalid SCXML, errors:\n%1").arg(msg));
        dataStream->close();
        dataStream->open(QBuffer::ReadOnly);
        sourceDecode(dataStream, errors, _w->getRegola());

        _w->_scxmlValidationErrors->setMessages(errors);
        _w->ui.messagePanel->setVisible(true);
    }
    delete dataStream;
    delete testMachine ;
    return returnValue ;
#endif
}
