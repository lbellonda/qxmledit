/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#ifndef XSLTNAVIGATORWIDGET_H
#define XSLTNAVIGATORWIDGET_H

#include "xmlEdit.h"

namespace Ui
{
class XSLTNavigatorWidget;
}

class XsltHelper ;

class XSLTNavigatorWidget : public QWidget
{
    Q_OBJECT
    bool _isInfoEnabled;

    void loadChildrenItems(QTreeWidgetItem *topItem, QHash<QString, Element*> &data, const QString &typeString);
    void loadTemplates(XsltHelper *xsltHelper, QTreeWidgetItem  *topItem);
    void loadFunctions(XsltHelper *xsltHelper, QTreeWidgetItem *topItem);
    Element *getSelectedItem();
public:
    explicit XSLTNavigatorWidget(QWidget *parent = 0);
    ~XSLTNavigatorWidget();

    void applyNewInfo(XsltHelper *xsltHelper);
    void setEnabledInfo(const bool isEnabled);
    bool isEnabledInfo();

signals:
    void goTo(Element *element);
    void edit(Element *element);

private:
    Ui::XSLTNavigatorWidget *ui;
private slots:
    void on_treeNavigator_currentItemChanged(QTreeWidgetItem * /*current*/, QTreeWidgetItem * /*previous*/);
    void on_showCmd_clicked();
    void on_treeNavigator_itemClicked(QTreeWidgetItem * /*item*/, int /*column*/);
    void on_editCmd_clicked();
    void on_treeNavigator_itemDoubleClicked(QTreeWidgetItem * /*item*/, int /*column*/);
};

#endif // XSLTNAVIGATORWIDGET_H
