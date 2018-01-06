/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2018 by Luca Bellonda and individual contributors  *
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


#ifndef PASTEATTRIBUTESDIALOG_H
#define PASTEATTRIBUTESDIALOG_H

#include <QDialog>
#include "libQXmlEdit_global.h"
#include "modules/uiutil/wchecker.h"

namespace Ui
{
class PasteAttributesDialog;
}

class CopyAttributesManager ;
class CopyAttributesSession ;
class Element ;
class Attribute ;

class LIBQXMLEDITSHARED_EXPORT PasteAttributesDialog : public QDialog
{
    Q_OBJECT

    Element *_element;
    CopyAttributesManager *_manager;
    CopyAttributesSession *_currentSession;
    UIHelperManager _uiManager;


    void retrieveSelection(QList<Attribute*> &resList);
    bool saveData();
    void accept();
    void selectAllItems(const bool select);
    void loadData();
    void loadStartData();
    void recalcUIState();
    void setupUI();
    void setupTable();
public:
    explicit PasteAttributesDialog(Element *element, CopyAttributesManager *manager, QWidget *parent = 0);
    ~PasteAttributesDialog();

    static void showDialog(Element *element, CopyAttributesManager *manager, QWidget *parent = 0);

private:
    Ui::PasteAttributesDialog *ui;
private slots:
    void on_cmdSelectAll_clicked();
    void on_cmdDeselectAll_clicked();
    void on_sessions_currentIndexChanged(int index);
    void on_cmdClearClipboard_clicked();
};

#endif // PASTEATTRIBUTESDIALOG_H
