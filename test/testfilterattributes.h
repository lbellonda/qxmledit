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


#ifndef TESTFILTERATTRIBUTES_H
#define TESTFILTERATTRIBUTES_H

#include <QString>
#include <QDateTime>
#include "sqlliteaccess.h"
#include "filter/attributeprofilemgmtdialog.h"
#include "filter/attributefilterdetaildialog.h"
#include "app.h"

class TestAttributeProfileMgmtDialog : public AttributeProfileMgmtDialog
{
public:
    bool hasBeenCalled;
    bool detailHasBeenCalled;
    bool hasBeenAccepted;

    explicit TestAttributeProfileMgmtDialog(UIDelegate *uiDelegate, AttributeFilterManagement *newManagement, QWidget *parent = 0);
    ~TestAttributeProfileMgmtDialog();

private:
    void showDetails(AttrFilterProfile *profile, AttrFilterDetail *detail);
    virtual AttributeFilterDetailDialog *getDetailDialog(AttrFilterProfile *profile, AttrFilterDetail *detail);
    virtual void accept();
};

class TestAttributeFilterDetailDialog : public AttributeFilterDetailDialog
{
public:
    TestAttributeProfileMgmtDialog *dlgParent;
    explicit TestAttributeFilterDetailDialog(QWidget *parent, UIDelegate *uiDelegate, AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail);
    virtual ~TestAttributeFilterDetailDialog();
    virtual int go();
    void setProfileData(const QString &newName, const QString &newDescr, const bool isWhiteList );
    void testInsertItem(const QString &newItemText);
    void editItem(const int rowIndex, const QString &newItemText);
    void deleteFirstItem();
};

class TestPData {
public:
    AttrFilterDetail *detail;
    AttrFilterProfile *profile;

    TestPData(const bool createData = false);
    ~TestPData();

    bool isReady();
};

class TestFilterAttributes
{
public:
    class FTest {
    public:
        TestAttributeProfileMgmtDialog *cfgDialog;
        App *app;

        FTest(TestAttributeProfileMgmtDialog *newCfgDialog, App *newApp);
    };
    class DTest {
    public:
        TestAttributeFilterDetailDialog *detDialog;
        App *app;

        DTest(TestAttributeProfileMgmtDialog *newCfgDialog, App *newApp);
    };
private:
    bool _isError ;
    QString _errorString;
    static QDateTime _now ;
   public:
    static QDateTime now() ;
private:
    bool error(const QString &testName, const QString &msg);

    bool testBaseMethods();
    bool testInsertOrUpdateProfile( SQLLiteTestAccess &access, int &idProfile, const bool isUpdate );
    bool testDeleteProfile( SQLLiteTestAccess &access );
public:
    static void deleteProfile(AttrFilterProfile *profile);
    static void deleteDetail(AttrFilterDetail *detail);
    static AttrFilterProfile *createProfile(const int id, const QString &name);
    static AttrFilterDetail *createDetail(const QString &name1, const QString &name2);
private:
    bool testBaseMethodsDetail();
    bool testBaseMethodsProfile();

    bool tfInsertData( SQLLiteTestAccess &access, TestPData *data1, TestPData *data2 );
    bool tfOpenProfileDialog( TestFilterAttributes::FTest &fTest );

    bool profileDialogSelectOne( TestFilterAttributes::FTest &fTest, const QString &testName, TestPData *data );
    bool profileDialogReadData( TestFilterAttributes::FTest &fTest, const QString &testName );
    bool profileDialogNew( TestFilterAttributes::FTest &fTest, const QString &testName );
    bool profileDialogEdit( TestFilterAttributes::FTest &fTest, const QString &testName );
    bool profileDialogDelete( TestFilterAttributes::FTest &fTest, const QString &testName );
    bool profileDialogChoose( TestFilterAttributes::FTest &fTest, const QString &testName );
    bool closeProfileDialog(TestFilterAttributes::FTest &fTest);

    bool saveDetail(AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail, const QString &newName, const QString &newDescr, const bool isWhiteList );
    bool testDetailEdit(const QString &testName, SessionDataInterface *access, const bool isInsert );
    bool testDetailEdit();
    bool testDetailInsert();
    bool testDetailDeleteItem();
    bool testDetailEditItem();

    // test the filter apply on elements

    bool parametrizedDetailTest(const QString &testName, bool (TestFilterAttributes::*oper) (TestAttributeFilterDetailDialog *dlg), AttrFilterProfile *expectedProfile, AttrFilterDetail *expectedDetail );

    bool operDeleteItem(TestAttributeFilterDetailDialog *dlg);
    bool operEditItem(TestAttributeFilterDetailDialog *dlg);

public:
    TestFilterAttributes();
    ~TestFilterAttributes();

    bool testStorage();
    bool testDialogApply();
    bool testFunctionality();
    bool testDetailFunctionality();
    bool testApply();
    bool testUnit();

};

#endif // TESTFILTERATTRIBUTES_H
