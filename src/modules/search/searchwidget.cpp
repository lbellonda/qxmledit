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


#include "searchwidget.h"
#include "ui_searchwidget.h"
#include "utils.h"
#include "config.h"
#include "modules/search/searchletmanager.h"

#define LIM_SEARCH_SIZE (10)

int SearchWidget::numInstances = 0 ;

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget)
{
    numInstances++;
    _appData = NULL;
    _id = numInstances ;
    _extendedMode = false ;
    _started = false;
    _internalStateOk = false;
    _manager = NULL ;
    _searchletManagerFactory = new SearchletManagerFactory();

    _isAdvancedSearch = false ;
    ui->setupUi(this);
    _internalStateOk = finishSetUpUi();
    if(!_internalStateOk) {
        Utils::error(tr("Error preparing the user interface."));
    }
    updateState(NULL);
    _started = true ;
    _internalStateOk = true ;
}

SearchWidget::~SearchWidget()
{
    void resetSearchletManagerFactory();
    delete ui;
}

void SearchWidget::resetSearchletManagerFactory()
{
    if(NULL != _searchletManagerFactory) {
        delete _searchletManagerFactory ;
        _searchletManagerFactory = NULL ;
    }
}

void SearchWidget::setSearchletManagerFactory(SearchletManagerFactory* newSearchletManagerFactory)
{
    resetSearchletManagerFactory();
    _searchletManagerFactory = newSearchletManagerFactory ;
}

void SearchWidget::updateState(FindTextParams *searchSettings)
{
    loadSearchSettings(searchSettings);
    enableSearchItems();
}

void SearchWidget::setManager(SearchManager *newManager)
{
    if(NULL != _manager) {
        return ;
    }
    _manager = newManager ;
    connect(_manager, SIGNAL(itemsChanged(const int)), this, SLOT(onItemsChanged(const int)));
    updateState(NULL);
}

void SearchWidget::setSettings(FindTextParams *searchSettings)
{
    updateState(searchSettings);
}

bool SearchWidget::finishSetUpUi()
{
    bool isOk = true;

    updateUI();

    connect(ui->closeSearchPanel, SIGNAL(clicked()), this, SLOT(onCloseSearchPanelClicked()));

    // connect to search widget. In the original code these actions were inseted into the UI
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(onFindText()));
    connect(ui->clearText, SIGNAL(clicked()), this, SLOT(clearSearchText()));
    connect(ui->counting, SIGNAL(clicked()), this, SLOT(onCountTextOccurrences()));
    connect(ui->useXQuery, SIGNAL(clicked()), this, SLOT(enableSearchItems()));
    connect(ui->cmdOpenAdvancedPanel, SIGNAL(clicked()), this, SLOT(onOpenAdvancedResultPanel()));
    connect(ui->cmdNext, SIGNAL(clicked()), this, SLOT(onSearchNext()));
    connect(ui->cmdPrev, SIGNAL(clicked()), this, SLOT(onSearchPrev()));

    _findCompleter = new LineEditWithCompleter(ui->searchBox);
    ui->searchBox->setDuplicatesEnabled(true);
    ui->searchBox->setInsertPolicy(QComboBox::NoInsert);
    ui->searchBox->setLineEdit(_findCompleter);
    ui->searchBox->installEventFilter(this);
    ui->searchBox->setCompleter(NULL);

    ui->searchLocation->insertItem(0, tr("All"), (int)FindTextParams::FIND_ALL);
    ui->searchLocation->insertItem(1, tr("Element Tag"), (int)FindTextParams::FIND_TAG);
    ui->searchLocation->insertItem(3, tr("Attribute values"), (int)FindTextParams::FIND_ATTRIBUTE_VALUE);
    ui->searchLocation->insertItem(2, tr("Attribute names"), (int)FindTextParams::FIND_ATTRIBUTE_NAME);
    ui->searchLocation->insertItem(4, tr("Text"), (int)FindTextParams::FIND_TEXT);
    ui->searchLocation->insertItem(5, tr("Comments"), (int)FindTextParams::FIND_COMMENT);
    ui->searchLocation->insertItem(5, tr("Text base 64 coded"), (int)FindTextParams::FIND_TEXT_BASE64);
    ui->searchLocation->setCurrentIndex(0);

    connect(ui->searchBox, SIGNAL(editTextChanged(QString)), this, SLOT(searchTextChanged(QString)));
    ui->clearText->setEnabled(false);
    //--
    QAction *recallSearchlets = new QAction(tr("Call searchlets"), this);
    connect(recallSearchlets, SIGNAL(triggered()), this, SLOT(onManageSearchlets()));
    connect(ui->cmdSearchlets, SIGNAL(clicked()), this, SLOT(onManageSearchlets()));
    ui->cmdSearchlets->addAction(recallSearchlets);
    //---
    _saveSearchletsAction = new QAction(tr("Save as searchlet"), this);
    _saveSearchletsAction->setEnabled(false);
    _saveSearchletsAction->setObjectName("saveSearchletsAction");
    connect(_saveSearchletsAction, SIGNAL(triggered()), this, SLOT(onSaveAsSearchlet()));
    ui->cmdSearchlets->addAction(_saveSearchletsAction);
    //--
    ui->cmdSearchlets->setEnabled(false);
    ui->cmdSearchlets->setVisible(false);

    return isOk;
}

void SearchWidget::setData(QXmlEditData *newData)
{
    _appData = newData;
    bool enableSearchlets = false;
    if((NULL != _appData) && (NULL != _appData->storageManager())) {
        enableSearchlets = true;
    }
    ui->cmdSearchlets->setEnabled(enableSearchlets);
    ui->cmdSearchlets->setVisible(enableSearchlets);
}

void SearchWidget::loadSearchSettings(FindTextParams *searchSettings)
{
    FindTextParams lastSearch;
    FindTextParams *params;
    if(NULL != searchSettings) {
        params = searchSettings ;
    } else {
        if(NULL != _manager) {
            params = _manager->params();
        } else {
            lastSearch.loadState();
            params = &lastSearch;
        }
    }
    updateSearchUI(params);
    loadSearchItems(ui->searchBox, ui->searchScope);
}

void SearchWidget::searchTextChanged(const QString & text)
{
    bool enabled = text.length() > 0 ;
    ui->clearText->setEnabled(enabled);
    _saveSearchletsAction->setEnabled(enabled);
    ui->occNumber->setText("");
}

void SearchWidget::updateSearchUI(const FindTextParams *lastSearch)
{
    ui->isHiglightAll->setChecked(lastSearch->isHiliteAll());
    ui->isMatchExactValue->setChecked(lastSearch->isIsMatchExact());
    ui->isCaseSensitive->setChecked(lastSearch->isCaseSensitive());
    ui->isOnlyChildren->setChecked(lastSearch->isLookOnlyChildren());
    ui->selectionToBookmarks->setChecked(lastSearch->isSelToBookmarks());
    ui->closeUnrelated->setChecked(lastSearch->isCloseUnrelated());
    ui->useXQuery->setChecked(lastSearch->useXQuery());
    ui->showSize->setChecked(lastSearch->isShowSize());
    Utils::selectComboValue(ui->searchLocation, lastSearch->getFindTarget());
    ui->lblSize->setVisible(lastSearch->isShowSize());
    ui->sizeOfSearch->setVisible(lastSearch->isShowSize());
    ui->wrapAround->setChecked(lastSearch->isWrapAround());
}

void SearchWidget::enableSearchItems()
{
    bool isXQuery = ui->useXQuery->isChecked();
    Utils::enableAndShowWidget(ui->searchScope, !isXQuery);
    Utils::enableAndShowWidget(ui->searchLocation, !isXQuery);
    Utils::enableAndShowWidget(ui->isMatchExactValue, !isXQuery);
    Utils::enableAndShowWidget(ui->isCaseSensitive, !isXQuery);
    Utils::enableAndShowWidget(ui->isHiglightAll, !isXQuery);
    Utils::enableAndShowWidget(ui->searchLabel, !isXQuery);
    Utils::enableAndShowWidget(ui->labelScope, !isXQuery);
    Utils::enableAndShowWidget(ui->textLabel, !isXQuery);
    Utils::enableAndShowWidget(ui->lblXPath, isXQuery);
    Utils::enableAndShowWidget(ui->wrapAround, !isXQuery);
}


void SearchWidget::loadSearchItems(QComboBox *comboItems, QComboBox *comboScopes)
{
    if(NULL == _manager) {
        return ;
    }
    QString currentTextComboItems = comboItems->currentText();
    comboItems->clear();
    comboItems->addItems(_manager->searchItems());
    comboItems->setEditText(currentTextComboItems);
    //---
    QString currentTextComboScope = comboScopes->currentText();
    comboScopes->clear();
    comboScopes->addItems(_manager->searchScopes());
    comboScopes->setEditText(currentTextComboScope);
}

void SearchWidget::registerSearchTerms(const QString &value, const QString &scope)
{
    if(NULL != _manager) {
        _manager->registerSearchTerms(_id, value, scope);
    }
}

void SearchWidget::setAdvancedSearch(const bool isAdvancedSearch)
{
    _isAdvancedSearch = isAdvancedSearch;
    updateUI();
}

void SearchWidget::updateUI()
{
    ui->cmdOpenAdvancedPanel->setVisible(_isAdvancedSearch);
}

bool SearchWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->searchBox) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(!keyEvent->isAutoRepeat()
                    && ((keyEvent->key() == Qt::Key_Enter) || (keyEvent->key() == Qt::Key_Return))) {
                ui->findButton->animateClick();
                return true ;
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}


void SearchWidget::setDataForCompletion(QSet<QString> *newData)
{
    _findCompleter->setData(newData);
}

void SearchWidget::onFindText()
{
    emit find();
}

void SearchWidget::onCountTextOccurrences()
{
    emit count();
}

void SearchWidget::onOpenAdvancedResultPanel()
{
    emit openAdvancedSearch();
}

void SearchWidget::clearSearchText()
{
    ui->searchBox->setEditText("");
    emit searchTextCleared();
}

FindTextParams* SearchWidget::getSearchParams(const FindTextParams::EFindType findType, const bool isFindOrCount, QList<Element*> *selection)
{
    FindTextParams::EFindTarget target = FindTextParams::FIND_ALL;
    int itemIndex = ui->searchLocation->currentIndex();
    if(itemIndex >= 0) {
        int trg = ui->searchLocation->itemData(itemIndex).toInt();
        target = (FindTextParams::EFindTarget) trg ;
    }

    FindTextParams *params = new FindTextParams(findType, ui->searchBox->currentText(), !isFindOrCount, ui->isHiglightAll->isChecked(), ui->isMatchExactValue->isChecked(),
            ui->isCaseSensitive->isChecked(), ui->isOnlyChildren->isChecked(), target,
            (isFindOrCount ? ui->selectionToBookmarks->isChecked() : false),
            (isFindOrCount ? ui->closeUnrelated->isChecked() : false),
            ui->showSize->isChecked(), ui->searchScope->currentText(), ui->wrapAround->isChecked(),
            ui->useXQuery->isChecked(), selection);

    if(NULL != _manager) {
        _manager->saveSearchSettings(params);
    }
    registerSearchTerms(ui->searchBox->currentText(), ui->searchScope->currentText());
    return params;
}

QString SearchWidget::messageCount()
{
    return _messageCount;
}

void SearchWidget::setSearchResults(FindTextParams* findArgs)
{
    int occurrences = findArgs->getOccurrences();
    if(occurrences > 0) {
        if(occurrences > 1) {
            _messageCount = QString(tr("Found %1 occurrences.")).arg(occurrences) ;
        } else {
            _messageCount = tr("Found 1 occurrence.") ;
        }
    } else {
        _messageCount = tr("No occurrences found.") ;
    }
    ui->occNumber->setText(_messageCount);
    if(findArgs->isShowSize()) {
        ui->sizeOfSearch->setText(QString(tr("%1")).arg(findArgs->size()));
    }
    ui->lblSize->setVisible(findArgs->isShowSize());
    ui->sizeOfSearch->setVisible(findArgs->isShowSize());
}

void SearchWidget::onCloseSearchPanelClicked()
{
    emit hiding();
}

void SearchWidget::regainFocus()
{
    ui->searchBox->setFocus(Qt::OtherFocusReason);
}

QString SearchWidget::currentText()
{
    return ui->searchBox->currentText();
}

void SearchWidget::setCurrentText(const QString &newText)
{
    ui->searchBox->setEditText(newText);
}

void SearchWidget::hideCloseWidget()
{
    ui->closeSearchPanel->setVisible(false);
}

void SearchWidget::onItemsChanged(const int key)
{
    if(key != _id) {
        // reload search combo leaving the same current data
        loadSearchItems(ui->searchBox, ui->searchScope);
    }
}

void SearchWidget::onManageSearchlets()
{
    SearchletManager *manager = _searchletManagerFactory->newSearchletManager();
    if(NULL != manager) {
        QString text = manager->chooseSearchlets(_appData, this);
        if(!text.isEmpty()) {
            ui->searchBox->setEditText(text);
        }
        delete manager ;
    }
}

void SearchWidget::onSaveAsSearchlet()
{
    SearchletManager *manager = _searchletManagerFactory->newSearchletManager();
    if(NULL != manager) {
        manager->insertSearchlet(this, _appData, ui->searchBox->currentText());
        delete manager ;
    }
}

void SearchWidget::onSearchNext()
{
    emit searchNext();
}

void SearchWidget::onSearchPrev()
{
    emit searchPrev();
}

void SearchWidget::setExtendedMode(const bool extendedMode)
{
    _extendedMode = extendedMode ;
    ui->cmdPrev->setVisible(!_extendedMode);
    ui->cmdNext->setVisible(!_extendedMode);
    ui->wrapAround->setVisible(!_extendedMode);
}
