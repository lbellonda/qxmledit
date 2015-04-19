/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef XSDFULLANNOTATIONSDIALOG_H
#define XSDFULLANNOTATIONSDIALOG_H

#include "modules/xsd/xsdannotationmodel.h"
#include <QDialog>
#include <QTableWidgetItem>

namespace Ui
{
class XSDFullAnnotationsDialog;
}

class XSDFullAnnotationsDialog : public QDialog
{
    Q_OBJECT

protected:
    XSDAnnotationModel *_model;

    void setupUI();
    void renderItem(const int row, XSchemaObject *srcObj);
    void setItem(const int row, const int column, const QString &text, XSchemaObject *src, const bool isEnabled);
    XInfoBase *currentSelectionAnnotation();
    void enableButtons();
    void renderModel(const bool retainSelection = false);
    void renderModel(const bool retainSelection, const bool setSelection, const int newSelectionIndex);

public:
    explicit XSDFullAnnotationsDialog(XSDAnnotationModel *newModel, QWidget *parent = 0);
    virtual ~XSDFullAnnotationsDialog();

    void addNewItem(XSchemaObject *newItem);
    void updateItem(const int index, XInfoBase *newItem);
    void deleteItem(const int index);

    XSDAnnotationModel *data();

private:
    Ui::XSDFullAnnotationsDialog *ui;
private slots:
    void on_cmdNew_clicked();
    void on_cmdModify_clicked();
    void on_cmdDelete_clicked();
    void onItemClicked(QTableWidgetItem *item);
    void onItemChanged(QTableWidgetItem *item);
    void on_annotations_itemDoubleClicked(QTableWidgetItem * item);
    void on_cmdMoveDown_clicked();
    void on_cmdMoveUp_clicked();
};

#endif // XSDFULLANNOTATIONSDIALOG_H
