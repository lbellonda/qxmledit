/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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

#ifndef SCXMLNAVIGATORWIDGET_H
#define SCXMLNAVIGATORWIDGET_H

#include "xmlEdit.h"

class SCXMLInfo;
class Element ;

namespace Ui
{
class SCXMLNavigatorWidget;
}

class SCXMLState;

class SCXMLNavigatorWidget : public QWidget
{
    Q_OBJECT
    SCXMLInfo *_info;
    bool _isInfoEnabled;
    bool _visIsFlat;
    QHash<Element*, QTreeWidgetItem*> _itemsByElement;
    bool _updatesEnabled;

    void reset();
    void loadState(SCXMLState *state, QTreeWidgetItem *parentItem, const bool isFlat);
    bool compareIfSameAndUpdateSchemas(SCXMLInfo *oldOne, SCXMLInfo *newOne);
    bool compareIfSameAndUpdateSchemas(QList<SCXMLState *> oldOne, QList<SCXMLState *> newOne);
    void redisplay(const bool isFlat);
    SCXMLState *getSelectedItem();
public:
    explicit SCXMLNavigatorWidget(QWidget *parent = 0);
    virtual ~SCXMLNavigatorWidget();

    void setEnabledInfo(const bool isEnabled);
    bool isEnabledInfo();
    void applyNewInfo(SCXMLInfo *info);
    void selectItem(Element *selection);

signals:
    void goToState(const QString &stateName, Element *element);
    void editState(const QString &stateName, Element *element);

private:
    Ui::SCXMLNavigatorWidget *ui;

private slots:
    void on_showCmd_clicked();
    void on_editCmd_clicked();
    void on_flat_clicked();
    void on_states_itemSelectionChanged();
    void on_states_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_states_itemClicked(QTreeWidgetItem *item, int column);
};

#endif // SCXMLNAVIGATORWIDGET_H
