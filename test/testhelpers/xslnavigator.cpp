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
#if 0
#include "modules/xslt/xsltnavigatordialog.h"
#include "ui_xsltnavigatordialog.h"

int XSLTNavigatorDialog::testToExecute=0;

bool XSLTNavigatorDialog::testPassed=false;
QString XSLTNavigatorDialog::testMsg;

void XSLTNavigatorDialog::testStart()
{
    testPassed = false;
    testMsg = "";
    switch(testToExecute) {
    case 0:
        testCheckNamesOrder();
        break;
    case 1:
        testEnableNavigate();
        break;
    case 2:
        testEnableNavigateToSelection();
        break;
    default:
        break;
    }
}

bool XSLTNavigatorDialog::testCheckChildItem(QTreeWidgetItem *item, const QString &expected, const int index)
{
    QString text = item->text(0);
    if( text != expected ) {
        testMsg = QString("Item %1, expected '%2', found '%3'").arg(index).arg(expected).arg(text);
        testPassed = false;
        return false;
    }
    return true;
}

bool XSLTNavigatorDialog::testCheckItem(QTreeWidgetItem *parent, const QString &expected, const int index)
{
    QTreeWidgetItem *child = parent->child(index);
    return testCheckChildItem(child, expected, index);
}

void XSLTNavigatorDialog::testCheckNamesOrder(QTreeWidget *tree)
{mancano i codici
    testPassed = true;
    bool templateFound = false;
    bool functionFound = false;
    int topLevelItems = ui->treeNavigator->topLevelItemCount();
    for(int i = 0 ; i < topLevelItems ; i ++ ) {
        QTreeWidgetItem * topLevel = tree->topLevelItem(i);
        if(topLevel->data(0, Qt::UserRole).toString() == "t" ) {
            templateFound = true;
            if(!testCheckItem(topLevel, "abcde", 0)) {return;}
            if(!testCheckItem(topLevel, "cdbfgfg", 1)) {return;}
            if(!testCheckItem(topLevel, "mgharf", 2)) {return;}
            if(!testCheckItem(topLevel, "one", 3)) {return;}
            if(!testCheckItem(topLevel, "zfkrjhf", 4)) {return;}
        } else if(topLevel->data(0, Qt::UserRole).toString() == "f" ) {
            functionFound = true;
            if(!testCheckItem(topLevel, "fabcde", 0)) {return;}
            if(!testCheckItem(topLevel, "fcdbfgfg", 1)) {return;}
            if(!testCheckItem(topLevel, "fmgharf", 2)) {return;}
            if(!testCheckItem(topLevel, "fzfkrjhf", 3)) {return;}
        }
    }
    if(!templateFound) {
        testPassed = false;
        testMsg = QString("No templates found");
    }
    if(!functionFound) {
        testPassed = false;
        testMsg = QString("No functions found");
    }
}

void XSLTNavigatorDialog::testEnableNavigate()
{
    testPassed = true;
    ui->treeNavigator->setCurrentItem(NULL);
    if( ui->cmdNavigate->isEnabled() ) {
        testPassed = false;
        testMsg = QString("Nav enabled without selection");
        return ;
    }
    QTreeWidgetItem * topLevel = ui->treeNavigator->topLevelItem(0);
    QTreeWidgetItem *child = topLevel->child(0);
    ui->treeNavigator->setCurrentItem(child);
    if( !ui->cmdNavigate->isEnabled() ) {
        testPassed = false;
        testMsg = QString("Nav not enabled with selection");
        return ;
    }
}

int XSLTNavigatorDialog::exec()
{
    accept();
    return 1;
}

void XSLTNavigatorDialog::testEnableNavigateToSelection()
{
    QList<QTreeWidgetItem *>  items = ui->treeNavigator->findItems("cdbfgfg", Qt::MatchFixedString|Qt::MatchRecursive, 0);
    if( items.count() == 1 ) {
        QTreeWidgetItem *  item = items.at(0);
        ui->treeNavigator->setCurrentItem(item);
        testPassed = true;
        on_cmdNavigate_clicked();
        accept();
    } else {
        reject();
    }
}
#endif
