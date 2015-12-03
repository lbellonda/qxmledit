/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "searchresultsdialog.h"
#include "ui_searchresultsdialog.h"
#include "xmlwidgetutilities.h"

#include "regola.h"
#include "qxmleditconfig.h"
#include "utils.h"

void SearchResultsDialog::go(QWidget *parent, QXmlEditData *appData, QList<Element*> &items, Regola *regola, Element *currentSelection, const QString &curText, FindTextParams* settings)
{
    SearchResultsDialog searchResults(parent, regola, currentSelection, appData, curText, settings);
    searchResults.setModal(true);
    searchResults.setXQueryResults(items);
    searchResults.show();
    searchResults.exec();
}

//--------------

SearchResultsDialog::SearchResultsDialog(QWidget *parent, Regola *regola, Element *currentSelection, QXmlEditData *appData, const QString &curText, FindTextParams* settings) :
    QDialog(parent),
    ui(new Ui::SearchResultsDialog)
{
    _appData = appData ;
    _regola = regola ;
    _currentSelection = currentSelection;
    _paintInfo = regola->getPaintInfo() ;
    ui->setupUi(this);
    ui->searchWidget->setExtendedMode(true);
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    finishSetup(settings);
    if(!curText.isEmpty()) {
        ui->searchWidget->setCurrentText(curText);
    }
    ui->searchWidget->setData(_appData);
}

SearchResultsDialog::~SearchResultsDialog()
{
    delete ui;
}


void SearchResultsDialog::setXQueryResults(QList<Element*> &nodes)
{
    Utils::TODO_NEXT_RELEASE("refactor");
    ui->treeWidget->clear();
    foreach(Element * node, nodes) {
        node->caricaFigli_to_refactor(ui->treeWidget, NULL, node->getParentRule()->getPaintInfo(), true, -1);
    }
}


void SearchResultsDialog::finishSetup(FindTextParams* settings)
{
    _helper.setTree(ui->treeWidget);
    ui->treeWidget->setAutoScroll(_appData->isAutoscroll());
    _helper.resetTree(ui->treeWidget, _paintInfo);
    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setUniformRowHeights(false);
    connect(ui->searchWidget, SIGNAL(find()), this, SLOT(findText()));
    connect(ui->searchWidget, SIGNAL(count()), this, SLOT(countTextOccurrences()));
    _helper.setUpTreeXmlDelegates(this, ui->treeWidget, getPaintInfo());
    ui->searchWidget->hideCloseWidget();
    ui->searchWidget->setManager(_appData->searchManager());
    ui->searchWidget->setSettings(settings);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeContextMenu(const QPoint &)));
    ui->treeWidget->installEventFilter(this);
}

PaintInfo* SearchResultsDialog::getPaintInfo()
{
    return _paintInfo;
}

void SearchResultsDialog::findText()
{
    findTextOperation(true);
}

void SearchResultsDialog::countTextOccurrences()
{
    findTextOperation(false);
}

void SearchResultsDialog::findTextOperation(const bool isFindOrCount)
{
    _results.clear();
    if(NULL != _regola) {

        setEnabled(false);
        ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();

        bool isErrorShown = false;
        bool isError = false;
        FindTextParams* findArgs = ui->searchWidget->getSearchParams(FindTextParams::FindAllOccurrences, isFindOrCount, &_results);
        if((NULL == findArgs) || ((NULL != findArgs) && !findArgs->checkParams(isErrorShown))) {
            isError = true ;
        }
        if(isError) {
            if(!isErrorShown) {
                Utils::error(this, tr("Impossible to start a search; please, check the parameters."));
            }
        } else {
            findArgs->saveState();
            _regola->unhiliteAll();
            findArgs->start();
            _regola->findText(*findArgs, _currentSelection);
            ui->searchWidget->setSearchResults(findArgs);
            setXQueryResults(_results);
        }
        if(NULL != findArgs) {
            delete findArgs;
        }
    }
    _helper.resizeTreeColumns(ui->treeWidget);
    ui->treeWidget->setUpdatesEnabled(true);
    setEnabled(true);
    Utils::restoreCursor();
}

void SearchResultsDialog::treeContextMenu(const QPoint& position)
{
    Element *element = _helper.getSelectedItem(ui->treeWidget);
    if(NULL != element) {
        QMenu contextMenu(this);
        QAction *copyAction = new QAction(tr("Copy"), &contextMenu);
        connect(copyAction, SIGNAL(triggered()), this, SLOT(on_copyAction_triggered()));
        contextMenu.addAction(copyAction);
        contextMenu.exec(ui->treeWidget->mapToGlobal(position));
    }
}

void SearchResultsDialog::on_copyAction_triggered()
{
    Element *element = _helper.getSelectedItem(ui->treeWidget);
    if(NULL != element) {
        QString clipboardText = element->getStringRepresentationForClipboard();
        _appData->setClipBoardItem(element->copyToClipboard(), clipboardText);
    }
}

bool SearchResultsDialog::eventFilter(QObject * obj, QEvent * event)
{
    return _helper.filterEvent(obj, event);
}

