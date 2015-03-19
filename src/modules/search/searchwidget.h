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


#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QComboBox>

namespace Ui
{
class SearchWidget;
}

#include "findtextparams.h"
#include "modules/widgets/lineeditwithcompleter.h"
#include "modules/search/searchmanager.h"
#include "modules/search/searchletmanager.h"
#include "qxmleditdata.h"

class SearchWidget : public QWidget
{
    Q_OBJECT

    static int numInstances;

    bool _started ;
    bool _internalStateOk ;
    bool _isAdvancedSearch ;
    LineEditWithCompleter *_findCompleter;
    QString _messageCount;
    SearchManager *_manager;
    int _id;
    QXmlEditData *_appData;
    QAction *_saveSearchletsAction;
    SearchletManagerFactory *_searchletManagerFactory;

public:
    explicit SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

    void setAdvancedSearch(const bool isAdvancedSearch);
    void setDataForCompletion(QSet<QString> *newData);
    FindTextParams* getSearchParams(const bool isFindOrCount, QList<Element*> *selection);
    void setSearchResults(FindTextParams* findArgs);
    QString messageCount();
    void regainFocus();
    QString currentText();
    void setCurrentText(const QString &newText);
    void hideCloseWidget();
    void setManager(SearchManager *newManager);
    void setSettings(FindTextParams *searchSettings);
    void setData(QXmlEditData *newData);
    void setSearchletManagerFactory(SearchletManagerFactory* newSearchletManagerFactory);

private:
    Ui::SearchWidget *ui;

    void updateUI();
    void registerSearchTerms(const QString &value, const QString &scope)  ;
    void loadSearchItems(QComboBox *comboItems, QComboBox *comboScopes) ;
    bool finishSetUpUi();
    void loadSearchSettings(FindTextParams *searchSettings);
    void saveSearchSettings(const FindTextParams &search);
    void updateSearchUI(const FindTextParams *lastSearch);
    bool eventFilter(QObject *obj, QEvent *event);
    void updateState(FindTextParams *searchSettings);
    void resetSearchletManagerFactory();


private slots:
    void searchTextChanged(const QString & text);
    void onFindText();
    void onCountTextOccurrences();
    void onOpenAdvancedResultPanel();
    void onCloseSearchPanelClicked();
    void clearSearchText();
    void enableSearchItems();
    void onItemsChanged(const int key);
    void onSaveAsSearchlet();
    void onManageSearchlets();

public:
signals:
    /**
     * @brief find
     * start a search
     */
    void find();
    /**
     * @brief openAdvancedSearch
     * request to open the advanced search
     */
    void openAdvancedSearch();
    /**
     * @brief count
     *Start a counting operation
     */
    void count();

    /**
     * @brief hiding
     * this panel is hiding
     */
    void hiding();


    /**
     * @brief searchTextCleared
     *useful for clear higlighting
     */
    void searchTextCleared();

};

#endif // SEARCHWIDGET_H
