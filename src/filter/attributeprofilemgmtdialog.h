/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef ATTRIBUTEPROFILEMGMTDIALOG_H
#define ATTRIBUTEPROFILEMGMTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "sessions/data_access/model/attrfilterprofile.h"
#include "attributefiltermanagement.h"
#include "UIDelegate.h"

class AttributeFilterDetailDialog;

namespace Ui
{
class AttributeProfileMgmtDialog;
}

class AttributeProfileMgmtDialog : public QDialog
{
    Q_OBJECT

protected:

    AttributeFilterManagement *_mgmt;
    QList<AttrFilterProfile*> _profiles;
    int _selectedProfileCode ;
    AttrFilterProfile *_selectedProfile;
    UIDelegate *_uiDelegate;

    void loadData();


public:
    explicit AttributeProfileMgmtDialog(UIDelegate *uiDelegate, AttributeFilterManagement *newManagement, QWidget *parent = 0);
    virtual ~AttributeProfileMgmtDialog();

    AttributeFilter *buildFilter();
    int selectedProfileCode() ;

private:
    Ui::AttributeProfileMgmtDialog *ui;

    void deleteProfiles();
    void fatalError(const QString &message);
    void enableUI();
    virtual AttributeFilterDetailDialog *getDetailDialog(AttrFilterProfile *profile, AttrFilterDetail *detail);
    AttrFilterProfile* profileFromItem(QListWidgetItem *item);

protected:
    virtual void showDetails(AttrFilterProfile *profile, AttrFilterDetail *detail);

private slots:
    void on_profilesList_currentItemChanged();
    void on_activateButton_clicked();
    void on_editButton_clicked();
    void on_newButton_clicked();
    void on_deleteButton_clicked();
    void on_profilesList_itemDoubleClicked(QListWidgetItem * item);


    friend class TestFilterAttributes;
};

#endif // ATTRIBUTEPROFILEMGMTDIALOG_H
