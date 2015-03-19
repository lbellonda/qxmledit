/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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


#ifndef ANONADVDIALOG_H
#define ANONADVDIALOG_H

#include <QDialog>

#include "regola.h"
#include "qxmleditdata.h"
#include "xmlwidgetutilities.h"
#include "anonprofile.h"
#include "anonymizeparameters.h"

class QTableWidgetItem;
class AnonProfileManagerFactory;

namespace Ui
{
class AnonAdvDialog;
}

class AnonAdvDialog : public QDialog
{
    Q_OBJECT

    Regola *_orig;
    AnonProfile *_profile;
    XmlWidgetUtilities _helper;
    PaintInfo *_paintInfo;
    QXmlEditData *_appData;
    bool _profileChanged;
    QString _lastFolder;
    QIcon _ex_in, _ex_in_r, _ex_ex_r, _ex_ex ;
    QIcon _ex_v_default, _ex_v_fixed ;
    GenericPersistentData *_profileData;
    AnonProfileManagerFactory *_profileManagerFactory;

    //----
    void endSetup();
    AnonymizeParameters *params();
    void addException(AnonException *exception, const bool update = true);
    QTableWidgetItem *newExItem(const int row, const int column, AnonException *exception, const QString &text);
    void loadData();
    void loadData(Regola *input);
    void updateThings(const bool changed = true);
    void enableItems();
    void onManageProfiles();
    AnonProfile* getProfileFromProfileData(GenericPersistentData *input);
    bool saveProfile();
    bool saveNewProfile();
    bool checkIfProfileChanged();
    bool onSaveProfile();
    bool onEditProfile();
    void setProfileData(GenericPersistentData *newData);
    void updateProfileData();
    void setChanged(const bool changed);
    int import(QTextStream &stream);
    QString cleanParmCSV(const QString &val);
    bool importUsingFile(const QString &filePath);

public:
    explicit AnonAdvDialog(QXmlEditData *appData, Regola *inputData, QWidget *parent = 0);
    ~AnonAdvDialog();

    AnonContext *getContext();

public Q_SLOTS:
    virtual void accept();
    virtual void reject();

private:
    Ui::AnonAdvDialog *ui;

private slots:
    void on_cmdExcElement_clicked();
    void on_cmdExcAttribute_clicked();
    void on_cmdExcText_clicked();
    void on_allChk_clicked(bool checked);
    void on_codeChk_clicked(bool checked);
    void on_chkFixedPattern_clicked(bool checked);
    void on_exceptions_currentItemChanged(QTableWidgetItem * current, QTableWidgetItem * previous);
    void on_treeWidget_reevaluateSelectionState();
    void on_cmdDeleteException_clicked();
    void on_cmdManageProfiles_clicked();
    void on_cmdSaveProfile_clicked();
    void on_cmdEditProfile_clicked();
    void on_actionExceptionElement_triggered();
    void on_actionExceptionAttributes_triggered();
    void on_actionExceptionText_triggered();
    void treeContextMenu(const QPoint &pt);
    void on_cmdImportException_clicked();
    void on_cmdExportExceptions_clicked();

    friend class TestAnonymize;
};

#endif // ANONADVDIALOG_H
