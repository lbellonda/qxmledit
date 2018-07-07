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

#include "searchcommanddialog.h"
#include "ui_searchcommanddialog.h"
#include "mainwindow.h"
#include "utils.h"

//----------------------------------------------------------------------------------------------------
class MenuSearchData
{
public:
    QString textToSearch;
    QString textToShow;
    QAction *action;

    MenuSearchData()
    {
        action = NULL ;
    }
};

//----------------------------------------------------------------------------------------------------

SearchCommandDialog::SearchCommandDialog(QList<QAction*> theActions, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchCommandDialog)
{
    _actionList = filterActions(theActions) ;
    _selectedAction = NULL ;
    buildActions();
    ui->setupUi(this);
    enableOK();
#if QT_VERSION >= QT_VERSION_CHECK(5,2,0)
    ui->search->setClearButtonEnabled(true);
#endif
    ui->search->setFocus();
    ui->commands->setUniformRowHeights(true);
}

SearchCommandDialog::~SearchCommandDialog()
{
    delete ui;
    EMPTYPTRLIST(_actions, MenuSearchData);
}

QAction *SearchCommandDialog::selectedAction()
{
    return _selectedAction ;
}

void SearchCommandDialog::on_search_textChanged(const QString &text)
{
    updateList(text);
}

void SearchCommandDialog::on_allWords_toggled()
{
    updateList(ui->search->text());
}

void SearchCommandDialog::updateList(const QString &text)
{
    const bool allWords = ui->allWords->isChecked();
    ui->commands->setUpdatesEnabled(false);
    ui->commands->clear();
    _selectedAction = NULL ;
    if(!text.trimmed().isEmpty()) {
        const QString & searchText = text.trimmed().toLower();
        const QStringList searchTerms = searchText.split(' ');
        const int termsCount = searchTerms.size();
        if(termsCount == 0) {
            return ;
        }
        foreach(MenuSearchData *data, _actions) {
            bool allMatches = true ;
            foreach(const QString &term, searchTerms) {
                if(allWords) {
                    if(!data->textToSearch.contains(term)) {
                        allMatches = false ;
                        break;
                    }
                } else {
                    if(data->textToSearch.contains(term)) {
                        addItem(data);
                        break;
                    }
                }
            } // for terms
            if(allWords && allMatches) {
                addItem(data);
            }
        } // menues
    } // if search
    ui->commands->resizeColumnToContents(0);
    ui->commands->resizeColumnToContents(1);
    ui->commands->setUpdatesEnabled(true);
    enableOK();
}

void SearchCommandDialog::addItem(MenuSearchData *data)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, data->textToShow);
    item->setText(1, data->action->toolTip());
    ui->commands->addTopLevelItem(item);
    item->setData(0, Qt::UserRole, qVariantFromValue((void*)data->action));
}

QString SearchCommandDialog::normalizeTextForSearch(const QString &text)
{
    QString newText = text.trimmed().toLower();
    // remove &
    newText = newText.replace("&", "");
    return newText ;
}

QString SearchCommandDialog::normalizeText(const QString &text)
{
    QString newText = text.trimmed();
    newText = newText.replace("&", "");
    newText = newText.replace("...", "");
    return newText ;
}

void SearchCommandDialog::buildActions()
{
    _actions.clear();
    foreach(QAction *action, _actionList) {
        if(filterAction(action)) {
            const QString & searchText = normalizeTextForSearch(action->text()) + QString(" ") + normalizeTextForSearch(action->toolTip());
            const QString & actionText = normalizeText(action->text());
            MenuSearchData * data = new MenuSearchData();
            data->textToShow = actionText ;
            data->textToSearch = searchText;
            data->action = action;
            _actions.append(data);
        }
    }
}

void SearchCommandDialog::enableOK()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(NULL != _selectedAction);
}

bool SearchCommandDialog::filterAction(QAction *action)
{
    if(action->data().toString() == MainWindow::ActionTagLastFiles) {
        return false;
    }
    if(!action->isEnabled() || !action->isVisible()) {
        return false;
    }
    return true ;
}

void SearchCommandDialog::on_commands_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)
{
    evalSelection();
    enableOK();
}

void SearchCommandDialog::on_commands_itemDoubleClicked(QTreeWidgetItem *, int)
{
    evalSelection();
    enableOK();
    accept();
}

void SearchCommandDialog::accept()
{
    if(NULL != _selectedAction) {
        QDialog::accept();
    }
}


QAction *SearchCommandDialog::evalSelection()
{
    _selectedAction = NULL ;
    if(ui->commands->selectedItems().size() > 0) {
        QTreeWidgetItem * item = ui->commands->selectedItems().at(0);
        if(NULL != item) {
            QVariant data = item->data(0, Qt::UserRole);
            QAction *action = (QAction*)data.value<void*>();
            _selectedAction = action ;
        }
    }
    return _selectedAction;
}

QList<QAction*> SearchCommandDialog::filterActions(QList<QAction*> sourceActions)
{
    QList<QAction*> result;
    foreach(QAction *action, sourceActions) {
        // filter
        if(action->objectName() != DYNAMIC_ACTION_NAME) {
            result.append(action);
        }
    }
    return result;
}

