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


#ifndef COPYATTRIBUTESDIALOG_H
#define COPYATTRIBUTESDIALOG_H

#include <QDialog>
#include "libQXmlEdit_global.h"
#include "element.h"
#include "modules/uiutil/wchecker.h"

namespace Ui
{
class CopyAttributesDialog;
}

class CopyAttributesManager ;

class LIBQXMLEDITSHARED_EXPORT CopyAttributesDialog : public QDialog
{
    Q_OBJECT

    friend class TestCopyAttrs;

    Element *_element;
    CopyAttributesManager *_manager;
    UIHelperManager _uiManager;

    void loadData();
    void retrieveSelection(QList<Attribute*> &resList);
    bool saveData();
    void selectAllItems(const bool select);
    void setupUi();
public:
    explicit CopyAttributesDialog(Element *element, CopyAttributesManager *manager, QWidget *parent = 0);
    ~CopyAttributesDialog();

    void accept();

    static void showDialog(Element *element, CopyAttributesManager *manager, QWidget *parent = 0);

private:
    Ui::CopyAttributesDialog *ui;
private slots:
    void on_cmdSelectAll_clicked();
    void on_cmdDeselectAll_clicked();

};

#endif // COPYATTRIBUTESDIALOG_H
