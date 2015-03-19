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


#ifndef XSLTNAVIGATORDIALOG_H
#define XSLTNAVIGATORDIALOG_H

#include "xmlEdit.h"

class XsltHelper;
class Element;

namespace Ui
{
class XSLTNavigatorDialog;
}

class XSLTNavigatorDialog : public QDialog
{
    Q_OBJECT

    XsltHelper *_helper;
    QTreeWidgetItem *_topTemplateItem;
    QTreeWidgetItem *_topFunctionsItem;
    Element *_elementChoosen;

    //---
#ifdef QXMLEDIT_TEST
#include "../test/testhelpers/xslnavigator.h"
#endif
    void finishSetup();
    void loadTemplates(QTreeWidgetItem  *topItem);
    void loadFunctions(QTreeWidgetItem  *topItem);
    void loadChildrenItems(QTreeWidgetItem *topItem, QSet<QString> &data, const QString &typeString, const QString &dataType);

public:
    explicit XSLTNavigatorDialog(XsltHelper *helper, QWidget *parent = 0);
    ~XSLTNavigatorDialog();

    Element *go();

private:
    Ui::XSLTNavigatorDialog *ui;
private slots:
    void on_treeNavigator_itemDoubleClicked(QTreeWidgetItem * item, int column);
    void on_treeNavigator_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void on_cmdNavigate_clicked();
};

#endif // XSLTNAVIGATORDIALOG_H
