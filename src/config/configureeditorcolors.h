/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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


#ifndef CONFIGUREEDITORCOLORS_H
#define CONFIGUREEDITORCOLORS_H

#include "xmlEdit.h"
#include <QDialog>
#include <QTreeWidgetItem>
#include "applicationdata.h"
#include "modules/services/colormanager.h"

namespace Ui
{
class ConfigureEditorColors;
}

class ConfigureEditorColors : public QDialog
{
    Q_OBJECT

    ApplicationData* _data;
    bool _started;
    QString _lastFolder;
    QString _fileFilter;

    void enableButtons();
    void loadList();
    QString translateKey(const QString &key);
    ColorEntry *selEntry();
    void updateEntry(QTreeWidgetItem *item);
    ColorEntry *entryOfItem(QTreeWidgetItem *item);
    QTreeWidgetItem *selItem();
    void loadSampleData();
    void updateSample();
protected:
    void import(QTextStream &stream);
    void writeExport(QTextStream &stream);

public:
    explicit ConfigureEditorColors(QWidget *parent = 0);
    ~ConfigureEditorColors();

    void init(ApplicationData* data);
    void saveIfChanged();

private:
    Ui::ConfigureEditorColors *ui;

private slots:
    void on_treeColors_itemSelectionChanged();
    void on_cmdResetColor_clicked();
    void on_cmdChangeColor_clicked();
    void on_cmdResetAllColors_clicked();
    void on_treeColors_itemDoubleClicked(QTreeWidgetItem * item, int column);
    void on_cmdExport_clicked();
    void on_cmdImport_clicked();
};

#endif // CONFIGUREEDITORCOLORS_H
