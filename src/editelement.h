/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef EDIT_ELEMENT_H
#define EDIT_ELEMENT_H

#include <QtGui>

#include <QDialog>
#include <QListWidgetItem>
#include "ui_EditElement.h"
#include "element.h"
#include "modules/delegates/attributecolumnitemdelegate.h"

class EditElement : public QDialog
{

    Q_OBJECT
protected:

    enum {
        T_COLUMN_CDATA = 1,
        T_COLUMN_TEXT = 2,
        T_COLUMN_MOD = 0
    };
    enum {
        A_COLUMN_NAME = 1,
        A_COLUMN_TEXT = 2,
        A_COLUMN_MOD = 0
    };

    bool _textModified;
    Element *target;
    bool isMixedContent;
    QColor modColor;
    bool isStarted ;
    AttributeColumnItemDelegate *_attributeDelegate;

public:
    EditElement(QWidget * parent = NULL);
    virtual ~EditElement();

    void setTarget(Element *pTarget);

    void sendDeleteCommand();
    void sendSelect(const int row);
    void sendModifyCommand(const QString &newData);
    void sendAddCommand(const QString &newData);
    void sendMoveUpCommand();
    void sendMoveDownCommand();
    void setEnableAllControls(const bool enableAllControls);

private:

    QListWidgetItem *loadTextNode(QListWidget *list, TextChunk *text);
    int appendTextNodeInTable(QTableWidget *table, const bool isEditable, const int desiredRow, const bool isCDATA, const QString &text, void * userData);
    int appendAttrNodeInTable(QTableWidget *table, const int desiredRow, const QString &name, const QString &value);
    void clearTextNodes();
    void addTextNode(TextChunk *text);

    void setAttrText(QListWidgetItem *item, const QString &name, const QString &value);
    void setUpdatedAttr(const int row);
    void setAttrFocus(const int row);
    void setUpdatedElement(const int row);
    void setTextToItem(QTableWidgetItem *item, const QString &text);
    QString textFromItem(QTableWidgetItem *item);

public slots:
    void accept();
private slots:
    void on_newAttribute_clicked();
    void on_delAttribute_clicked();
    void on_editTag_textChanged(const QString &);
    void on_attrTable_itemSelectionChanged();
    void on_elementTable_itemSelectionChanged();
    void on_textAdd_clicked();
    void on_textDel_clicked();
    void on_textUp_clicked();
    void on_textDown_clicked();
    void on_textEdit_clicked();
    void on_elementTable_cellClicked(int row, int column);
    void on_attrTable_itemChanged(QTableWidgetItem * item);
    void on_cmdFromBase64_clicked();
    void on_cmdToBase64_clicked();

private:
    Ui::Dialog ui;
    void error(const QString& message);
    void errorNoAttrData();
    void errorNoAttrSel();
    bool validateAttr(const QString &name);
    bool checkTagSyntax(const QString &theTag);
    void enableOK();

    void moveDown(QTableWidget *table);
    void moveUp(QTableWidget *table);
    static void swapRow2(QTableWidget *table, const int rowStart, const int rowEnd);
    int getNextRow(const int currentRow);
    Element * getUserData(QTableWidgetItem *item);
    bool isElementText(QTableWidgetItem *item);
protected:
    void doBase64Operation(const bool isFromBase64);

};

#endif
