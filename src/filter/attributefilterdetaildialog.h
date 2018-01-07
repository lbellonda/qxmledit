/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef ATTRIBUTEFILTERDETAILDIALOG_H
#define ATTRIBUTEFILTERDETAILDIALOG_H

#include <QDialog>

#include "sessions/data_access/model/attrfilterprofile.h"
#include "attributefiltermanagement.h"
#include "UIDelegate.h"

namespace Ui
{
class AttributeFilterDetailDialog;
}

class AttributeFilterDetailDialog : public QDialog
{
    Q_OBJECT

    AttributeFilterManagement *_mgmt;
    AttrFilterProfile *_profile;
    AttrFilterDetail *_detail;
protected:
    UIDelegate *_uiDelegate;

public:
    explicit AttributeFilterDetailDialog(QWidget *parent, UIDelegate *uiDelegate, AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail);
    virtual ~AttributeFilterDetailDialog();

    virtual int go();
protected:
    Ui::AttributeFilterDetailDialog *ui;
private:
    bool save(AttrFilterProfile *profile, AttrFilterDetail *detail);
    bool validate(AttrFilterProfile *profile, AttrFilterDetail *detail);
    void loadData();
    void enableUI();
protected:
    void fatalError(const QString &message);

public slots:
    virtual void accept();

private slots:
    void on_namesList_itemSelectionChanged();
    void on_txtName_textChanged(const QString & text);
    void on_editCmd_clicked();
    void on_deleteCmd_clicked();
    void on_newCmd_clicked();
};

#endif // ATTRIBUTEFILTERDETAILDIALOG_H
