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


#ifndef ANONATTREXCDIALOG_H
#define ANONATTREXCDIALOG_H

#include <QDialog>
#include "element.h"
#include "modules/anonymize/anonexception.h"
#include "modules/uiutil/wchecker.h"

namespace Ui
{
class AnonAttrExcDialog;
}

class AnonSettingsWidget;

class AnonAttrExcDialog : public QDialog
{
    Q_OBJECT

    Element *_element;
    UIHelperManager _uiManager;

    void loadData();
    void retrieveSelection(QList<Attribute*> &resList);
    void selectAllItems(const bool select);
    QString makePath(const QString &elementPath, const QString &attributeName);
    AnonException *getAnException(Attribute *attribute);
    void setupUi();

public:
    explicit AnonAttrExcDialog(Element *element, QWidget *parent = 0);
    ~AnonAttrExcDialog();
    void accept();

    QList<AnonException*> getExceptions();
    AnonSettingsWidget *settingsWidget();
    void selectAllAttributes();

private:
    Ui::AnonAttrExcDialog *ui;
private slots:
    void on_cmdSelectAll_clicked();
    void on_cmdDeselectAll_clicked();

    friend class TestAnonymize;

};

#endif // ANONATTREXCDIALOG_H
