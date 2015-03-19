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

#include "testfilterattributes.h"
#include "attributefilterdialog.h"
#include <QTreeWidgetItem>
#include <QTest>
#include "ui_attributeprofilemgmtdialog.h"
#include "ui_attributefilterdetaildialog.h"


#define TEST_FILE1 "../test/data/testattr.xml"


TestFilterAttributes::FTest::FTest(TestAttributeProfileMgmtDialog *newCfgDialog, App *newApp) : cfgDialog(newCfgDialog), app(newApp)
{
}



TestPData::TestPData(const bool createData)
{
    if( createData ) {
        profile = TestFilterAttributes::createProfile(0, "a");
        detail = TestFilterAttributes::createDetail("name1", "name2");
    } else {
        profile = NULL ;
        detail = NULL ;
    }
}


TestPData::~TestPData()
{
    if(NULL != profile) {
        delete profile ;
    }
    if(NULL != detail) {
        delete detail ;
    }
}


bool TestPData::isReady()
{
    if( ( NULL == profile ) || ( NULL == detail ) ) {
        return false;
    }
    return true ;
}

QDateTime TestFilterAttributes::_now = QDateTime::currentDateTime() ;

TestFilterAttributes::TestFilterAttributes()
{
    _isError = false;
}

TestFilterAttributes::~TestFilterAttributes()
{
}

QDateTime TestFilterAttributes::now()
{
    return _now ;
}

bool TestFilterAttributes::error(const QString &testName, const QString &msg)
{
    _errorString = QString("%1: %2").arg(testName).arg(msg);
    _isError = true ;
    return false ;
}

bool TestFilterAttributes::testBaseMethodsDetail()
{
    QString testName = "testBaseMethodsDetail";

    AttrFilterDetail *detail1 = createDetail("name a", "name");
    AttrFilterDetail *detail2 = createDetail("name b", "name");
    AttrFilterDetail *detail3 = createDetail("name b", "");
    AttrFilterDetail *detail4 = createDetail("name a", "name");

    if( detail1->compareTo(NULL)) {
        return error(testName, "detail compare to null" );
    }
    if( !detail1->compareTo(detail1)) {
        return error(testName, "detail compare same object" );
    }
    if( detail1->compareTo(detail2)) {
        return error(testName, "detail compare to same size false" );
    }
    if( detail3->names().size() != 1 ) {
        return error(testName, "detail bad size data." );
    }
    if( detail1->compareTo(detail3)) {
        return error(testName, "detail compare to different size" );
    }
    if( !detail1->compareTo(detail4)) {
        return error(testName, "detail compare equals" );
    }

    deleteDetail(detail1);
    deleteDetail(detail2);
    deleteDetail(detail3);
    deleteDetail(detail4);

    return true ;
}

bool TestFilterAttributes::testBaseMethodsProfile()
{
    QString testName = "testBaseMethodsProfile";

    AttrFilterProfile *profile1 = createProfile( 1, "name");
    AttrFilterProfile *profile2 = createProfile( 1, "name");
    AttrFilterProfile *profile3 = createProfile( 2, "name");
    AttrFilterProfile *profile4 = createProfile( 1, "name");
    AttrFilterProfile *profile5 = createProfile( 1, "name");
    AttrFilterProfile *profile6 = createProfile( 1, "name");
    AttrFilterProfile *profile7 = createProfile( 1, "name");
    AttrFilterProfile *profile8 = createProfile( 1, "name");

    if( !profile1->compareTo(profile1)) {
        return error(testName, "profile compare to itself" );
    }

    if( profile1->compareTo(NULL)) {
        return error(testName, "profile compare to null" );
    }
    if( profile1->compareToBase(NULL)) {
        return error(testName, "profile compare b to null" );
    }
    //--
    if( !profile1->compareTo(profile2)) {
        return error(testName, "profile compare to equals" );
    }
    if( !profile1->compareToBase(profile2)) {
        return error(testName, "profile compare b to equals" );
    }
    //--
    profile3->setId(100);
    if( profile1->compareTo(profile3)) {
        return error(testName, "profile compare id" );
    }
    if( profile1->compareToBase(profile3)) {
        return error(testName, "profile compare b id" );
    }
    //--
    profile4->setName("x");
    if( profile1->compareTo(profile4)) {
        return error(testName, "profile compare name" );
    }
    if( profile1->compareToBase(profile4)) {
        return error(testName, "profile compare b name" );
    }
    //--
    profile5->setDescription("x");
    if( profile1->compareTo(profile5)) {
        return error(testName, "profile compare description" );
    }
    if( profile1->compareToBase(profile5)) {
        return error(testName, "profile compare b description" );
    }
    //--
    profile1->setWhiteList(true);
    profile6->setWhiteList(false);
    if( profile1->compareTo(profile6)) {
        return error(testName, "profile compare whitelist" );
    }
    if( profile1->compareToBase(profile6)) {
        return error(testName, "profile compare whitelist" );
    }
    //--
    QDateTime dtUpdate;
    dtUpdate.setMSecsSinceEpoch(profile1->updateTime().toMSecsSinceEpoch()+1000);
    profile7->setUpdateTime(dtUpdate);
    if( profile1->compareTo(profile7)) {
        return error(testName, "profile compare update time" );
    }
    //--
    QDateTime dtCreation;
    dtCreation.setMSecsSinceEpoch(profile1->creationTime().toMSecsSinceEpoch()+1000);
    profile8->setCreationTime(dtCreation);
    if( profile1->compareTo(profile8)) {
        return error(testName, "profile compare creation time" );
    }
    //---
    dtUpdate.setMSecsSinceEpoch(profile1->updateTime().toMSecsSinceEpoch()+10000);
    dtCreation.setMSecsSinceEpoch(profile1->creationTime().toMSecsSinceEpoch()+10000);
    profile8->setUpdateTime(dtUpdate);
    profile8->setCreationTime(dtCreation);
    if( profile1->compareTo(profile8)) {
        return error(testName, "profile compare times " );
    }
    if( !profile1->compareToBase(profile8)) {
        return error(testName, "profile compare base time differ" );
    }

    deleteProfile(profile1);
    deleteProfile(profile2);
    deleteProfile(profile3);
    deleteProfile(profile4);
    deleteProfile(profile5);
    deleteProfile(profile6);
    deleteProfile(profile7);
    deleteProfile(profile8);

    return true;
}


bool TestFilterAttributes::testBaseMethods()
{
    bool isOk = true ;
    if( !testBaseMethodsDetail() ) {
        isOk = false;
    }
    if( !testBaseMethodsProfile() ) {
        isOk = false;
    }
    return isOk;
}

void TestFilterAttributes::deleteProfile(AttrFilterProfile *profile)
{
    if(NULL != profile) {
        delete profile ;
    }
}

void TestFilterAttributes::deleteDetail(AttrFilterDetail *detail)
{
    if(NULL != detail) {
        delete detail ;
    }
}

AttrFilterProfile *TestFilterAttributes::createProfile(const int id, const QString &name)
{
    AttrFilterProfile *profile = new AttrFilterProfile();
    if( NULL != profile ) {
        profile->setId(id);
        profile->setName(name);
        profile->setWhiteList(true);
        profile->setDescription("description");
        profile->setCreationTime(now());
        profile->setUpdateTime(now());
    }
    return profile;
}

AttrFilterDetail *TestFilterAttributes::createDetail(const QString &name1, const QString &name2)
{
    AttrFilterDetail *detail = new AttrFilterDetail();
    if( NULL != detail ) {
        if( !name1.isEmpty() ) {
            detail->addName(name1);
        }
        if( !name2.isEmpty() ) {
            detail->addName(name2);
        }
    }
    return detail;
}

/*
Da tenere?
bool TestFilterAttributes::insertProfile( const QString &testName, SQLLiteTestAccess &access, AttrFilterProfile *profile, AttrFilterDetail *detail )
{
    DataResult result;
    access.access->saveProfile(result, profile, detail );
    if( !result.isOk() ) {
        status = error(testName, QString("insert profile: %1").arg(result.message()));
    }
}
*/

bool TestFilterAttributes::testInsertOrUpdateProfile( SQLLiteTestAccess &access, int &idProfile, const bool isUpdate )
{
    QString testName = isUpdate? "updateProfile" : "insertProfile";

    bool status = true;
    TestPData p1;
    TestPData p2;
    p1.profile = isUpdate? createProfile(idProfile, "newName2") :createProfile(0, "a");
    p1.detail = isUpdate? createDetail("name1", "") : createDetail("name a", "name aa");
    p2.profile = createProfile(0, "b");
    p2.detail = createDetail("bbb", "");

    if( !p1.isReady() || ( isUpdate && !p2.isReady() ) ) {
        return error(testName, "alloc objects");
    }

    QDateTime tmUpdateBefore = p1.profile->updateTime();
    QDateTime tmCreateBefore = p1.profile->creationTime();

    DataResult result;
    access.access->attributeNamesFilterSaveProfile(result, p1.profile, p1.detail );
    if( !result.isOk() ) {
        return error(testName, QString("insert profile: %1").arg(result.message()));
    }
    if( p1.profile->updateTime() < tmUpdateBefore ) {
        return error(testName, QString("insert profile, update time invalid lower"));
    }
    if( p1.profile->updateTime() > QDateTime::currentDateTime() ) {
        return error(testName, QString("insert profile, update time invalid sup"));
    }
    if( isUpdate ) {
        if( p1.profile->creationTime() != tmCreateBefore ) {
            return error(testName, QString("update profile, creation time invalid"));
        }
    } else {
        if( p1.profile->creationTime() < tmCreateBefore ) {
            return error(testName, QString("insert profile, creation time invalid lower"));
        }
        if( p1.profile->creationTime() > QDateTime::currentDateTime() ) {
            return error(testName, QString("insert profile, creation time invalid sup"));
        }
    }

    // Insert some data more to test foreign key selectivity.
    DataResult resultS;
    access.access->attributeNamesFilterSaveProfile(resultS, p2.profile, p2.detail );
    if( !resultS.isOk() ) {
        return error(testName, QString("insert profile 2: %1").arg(resultS.message()));
    }
    if( p1.profile->id() == 0 ) {
        return error(testName, QString("insert profile id not read"));
    }
    if( !isUpdate ) {
        idProfile = p1.profile->id() ;
    }
    DataResult result2;
    QList<AttrFilterProfile*> readList = access.access->attributeNamesFilterReadProfiles(result2);
    if( !result2.isOk() ) {
        return error(testName, QString("read profile list failed: %1").arg(result2.message()));
    }
    int expected = isUpdate? 3 : 2 ;
    if( readList.size() == expected ) {
        bool found = false;
        AttrFilterProfile* profileRead = NULL ;
        foreach( AttrFilterProfile* profileR, readList ) {
            if( profileR->id() == p1.profile->id() ) {
                profileRead  = profileR;
                found = true ;
            }
        }
        if( !found ) {
            status = error(testName, "after write, the profile does not show");
        } else {
            // change the creation time for comparison
            if( isUpdate ) {
                p1.profile->setCreationTime(profileRead->creationTime());
            }
            if( !profileRead->compareTo(p1.profile) ) {
                status = error(testName, "after insert, compare failed");
            }
        }
    } else {
        status = error(testName, QString("read profile, expected %1, found %2").arg(expected).arg(readList.size()) );
    } // test list

    foreach( AttrFilterProfile* p, readList ) {
        delete p ;
    }
    // test for details
    DataResult resultd;
    AttrFilterDetail *detailRead = access.access->attributeNamesFilterReadDetail(resultd, idProfile);
    if( NULL == detailRead ) {
        return error( testName, "read detail null" );
    }
    if( ! detailRead->compareTo(p1.detail) ) {
        status = error( testName, "detail read not equal to the saved one" );
    }
    deleteDetail(detailRead);
    return status;
}

/*
bool TestFilterAttributes::AAAtestUpdateProfile( SQLLiteTestAccess &access )_da eliminare
{
    QString testName = "updateProfile";

    bool status = true;
    AttrFilterProfile *profile1 = createProfile(0, "a");
    AttrFilterProfile *profile2 = createProfile(0, "b");
    AttrFilterDetail *detail1 = createDetail("name a", "name aa");
    AttrFilterDetail *detail2 = createDetail("name b", "name bb");

    int idProfile = 0;
    if( ( NULL == profile1 ) || ( NULL == profile2 )
            || ( NULL == detail1 ) || ( NULL == detail2 ) ) {
        status = error(testName, "alloc objects");
    } else {
        DataResult result;
        access.access->saveProfile(result, profile1, detail1 );
        if( !result.isOk() ) {
            status = error(testName, QString("insert profile: %1").arg(result.message()));
        }
        DataResult resultS;
        access.access->saveProfile(resultS, profile2, detail2 );
        if( !resultS.isOk() ) {
            status = error(testName, QString("insert profile 2: %1").arg(resultS.message()));
        } else {
            if( profile1->id() == 0 ) {
                status = error(testName, QString("insert profile id not read"));
            } else {
                idProfile = profile1->id() ;
                DataResult result2;
                QList<AttrFilterProfile*> readList = access.access->attributeNamesFilterReadProfile(result2);
                if( !result2.isOk() ) {
                    status = error(testName, QString("read profile list failed: %1").arg(result2.message()));
                } else {
                    if( readList.size() !=  1) {
                        AttrFilterProfile* profileRead = readList.at(0);
                        // change the update time for comparison
                        profile1->setUpdateTime(profileRead->updateTime());
                        if( ! profileRead->compareTo(profile1) ) {
                            status = error(testName, "after insert, compare failed");
                        }
                    }
                } // test list
                foreach( AttrFilterProfile* p, readList ) {
                    delete p ;
                }
            }
        }
        // test for details
        DataResult resultd;
        AttrFilterDetail *detailRead = attributeNamesFilterReadDetail(resultd, idProfile);
        if( NULL == detailRead ) {
            status = error( testName, "read detail null" );
        } else {
            if( ! detailRead->compareTo(detail1) ) {
                status = error( testName, "detail read not equal to the saved one" );
            }
        }
        deleteDetail(detailRead);
    }
    deleteProfile(profile1);
    deleteProfile(profile2);
    deleteDetail(detail1);
    deleteDetail(detail2);
    return false;
}
*/

/*
bool TestFilterAttributes::testDeleteProfileEmpty( SQLLiteTestAccess &access )
{
    QString testName = "DeleteProfileEmpty";

    bool status = true;
    DataResult result;
    access.access->attributeNamesFilterDeleteProfile(result, -1);
    if( !result2.isOk() ) {
        status = error(testName, QString("delete non existent profile failed: %1").arg(result2.message()));
    } else {
        DataResult result2;
        QList<AttrFilterProfile*> readList = access.access->attributeNamesFilterReadProfile(result2);
        if( !result2.isOk() ) {
            status = error(testName, QString("read profile list failed: %1").arg(result2.message()));
        } else {
            if( !readList.isEmpty()) {
                status = error(testName, QString("delete non existent profile return ko: %1").arg(result2.message()));
            }
            foreach( AttrFilterProfile* p, readList ) {
                delete p ;
            }
        }
    }
    DataResult resultd;
    AttrFilterDetail *detailRead = attributeNamesFilterReadDetail(resultd, idProfile);
    if( NULL == detailRead ) {
        status = error( testName, "read detail null" );
    } else {
        if( ! detailRead->compareTo(detail1) ) {
            status = error( testName, "detail read not equal to the saved one" );
        }

        da finire
    }
    deleteDetail(detailRead);
}
    deleteProfile(profile1);
    deleteProfile(profile2);
    deleteDetail(detail1);
    deleteDetail(detail2);
    return false;
}
*/

bool TestFilterAttributes::testDeleteProfile( SQLLiteTestAccess &access )
{
    QString testName = "deleteProfile";

    bool status = true;
    AttrFilterProfile *profile1 = createProfile(0, "a");
    AttrFilterDetail *detail1 = createDetail("name a", "name aa");

    int idProfile = 0;
    if( ( NULL == profile1 ) || ( NULL == detail1 ) ) {
        status = error(testName, "alloc objects");
    } else {
        DataResult result;
        access.access->attributeNamesFilterSaveProfile(result, profile1, detail1 );
        if( !result.isOk() ) {
            status = error(testName, QString("insert profile: %1").arg(result.message()));
        } else {
            DataResult resultD;
            idProfile = profile1->id();
            access.access->attributeNamesFilterDeleteProfile(resultD, profile1->id());
            if( !resultD.isOk() ) {
                status = error(testName, QString("delete non profile failed: %1").arg(resultD.message()));
            } else {
                DataResult result2;
                QList<AttrFilterProfile*> readList = access.access->attributeNamesFilterReadProfiles(result2);
                if( !result2.isOk() ) {
                    status = error(testName, QString("read profile list failed: %1").arg(result2.message()));
                } else {
                    if( !readList.isEmpty()) {
                        status = error(testName, QString("read profile after delete ko, %1 result").arg(readList.size()));
                    }
                } // test list
                foreach( AttrFilterProfile* p, readList ) {
                    delete p ;
                }
            }
        }
        // test for details
        if( idProfile > 0 ) {
            DataResult resultd;
            AttrFilterDetail *detailRead = access.access->attributeNamesFilterReadDetail(resultd, idProfile);
            if( ( NULL != detailRead ) && ( detailRead->names().length() > 0 ) ) {
                status = error( testName, "read detail not null/empty after delete" );
            }
            deleteDetail(detailRead);
        } else {
            status = error( testName, "read detail after delete not executed" );
        }
    }
    deleteProfile(profile1);
    deleteDetail(detail1);
    return status ;
}

bool TestFilterAttributes::testUnit()
{
    if( !testBaseMethods() ) {
        return false;
    }
    return true;
}


bool TestFilterAttributes::testStorage()
{
    QString testName = "storage";

    // Fixtures
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init data access");
    }
    int idProfile = -1 ;
    if( !testInsertOrUpdateProfile( access, idProfile, false ) ) {
        return false;
    }
    if( !testInsertOrUpdateProfile( access, idProfile, true ) ) {
        return false;
    }

    SQLLiteTestAccess access2;
    if(!access2.init()) {
        return error(testName, "init data access - delete");
    }
    if( !testDeleteProfile( access2 ) ) {
        return false;
    }

    return true;
}

// test the filter apply on elements
bool TestFilterAttributes::testApply()
{
    QString msg1 = "apply test" ;
    App app;
    if(!app.init() ) {
        return error(msg1, "-1");
    }
    app.mainWindow()->loadFile(QString(TEST_FILE1));

    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection.append(1);
    selection.append(0);
    Element *selectedElement ;
    selectedElement = regola->findElementByArray(selection);
    if( NULL != selectedElement ) {
        return error(msg1, "7");
    }

    XmlEditWidget *editor = app.mainWindow()->getEditor();
    editor->setCurrentItem(selectedElement);
    AttributeFilter attributeFilter;
    PaintInfo *paintInfo = editor->getPaintInfo();
    QTreeWidgetItem *item = editor->getSelItem();
    QString str = item->text(paintInfo->columnForAttributes);
    if( !str.contains("attr1=") ) {
        return error(msg1, "1");
    }
    QIcon icon = item->icon(paintInfo->columnForAttributes);
    if(!icon.isNull()) {
        return error(msg1, "2");
    }
    attributeFilter.attributeNames().insert("attr2");
    paintInfo->setAttributeFilter(&attributeFilter);
    editor->repaint();
    str = item->text(paintInfo->columnForAttributes);
    if( str.contains("attr1=") ) {
        return error(msg1, "3");
    }
    icon = item->icon(paintInfo->columnForAttributes);
    if(icon.isNull()) {
        return error(msg1, "4");
    }
    paintInfo->setAttributeFilter(NULL);
    editor->repaint();
    str = item->text(paintInfo->columnForAttributes);
    if( !str.contains("attr1=") ) {
        return error(msg1, "5");
    }
    icon = item->icon(paintInfo->columnForAttributes);
    if(!icon.isNull()) {
        return error(msg1, "6");
    }
    return true ;
}

// test the attribute filter dialog
bool TestFilterAttributes::testDialogApply()
{
    QString msgTest = "test DialogApply" ;
    App app;
    if(!app.init() ) {
        return error(msgTest, "-1");
    }
    app.mainWindow()->loadFile(QString(TEST_FILE1));

    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection.append(1);
    selection.append(0);
    Element *selectedElement ;
    selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(msgTest, "7");
    }
    XmlEditWidget *editor = app.mainWindow()->getEditor();
    editor->setCurrentItem(selectedElement);

    AttributeFilterDialog dialog(app.mainWindow(), selectedElement) ;
    dialog.show();
    QTest::qWait(100);
    dialog.accept();

    AttributeFilter* newFilter = dialog.buildFilter();
    QScopedPointer<AttributeFilter> newFilterSP(newFilter);
    if(NULL == newFilter) {
        return error(msgTest, "1" );
    }
    newFilter->setTemporary(true);
    app.mainWindow()->setAttributeProfile(newFilter);
    if( !app.mainWindow()->_attributeFilterWidget->isVisible() ) {
        return error(msgTest, "1a" );
    }
    app.mainWindow()->setAttributeProfile(NULL);
    if( app.mainWindow()->_attributeFilterWidget->isVisible() ) {
        return error(msgTest, "1b" );
    }

    PaintInfo *paintInfo = editor->getPaintInfo();
    QTreeWidgetItem *item = editor->getSelItem();
    QString str = item->text(paintInfo->columnForAttributes);
    if( !str.contains("attr1=") ) {
        return error(msgTest, "1");
    }
    QIcon icon = item->icon(paintInfo->columnForAttributes);
    if(!icon.isNull()) {
        return error(msgTest, "2");
    }
    paintInfo->setAttributeFilter(newFilter);
    editor->repaint();
    str = item->text(paintInfo->columnForAttributes);
    if( str.contains("attr1=") ) {
        return error(msgTest, "3");
    }
    icon = item->icon(paintInfo->columnForAttributes);
    if(icon.isNull()) {
        return error(msgTest, "4");
    }
    paintInfo->setAttributeFilter(NULL);
    editor->repaint();
    str = item->text(paintInfo->columnForAttributes);
    if( !str.contains("attr1=") ) {
        return error(msgTest, "5");
    }
    icon = item->icon(paintInfo->columnForAttributes);
    if(!icon.isNull()) {
        return error(msgTest, "6");
    }
    return true ;
}


/****** TODO: check if useful
bool TestFilterAttributes::aaaa()
{
    QString msgTest = "aaa" ;
    App app;
    if(!app.init() ) {
        return error(msg1, "-1");
    }
    app.mainWindow()->loadFile(QString(TEST_FILE1));

    Regola *regola = app.mainWindow()->getRegola();
    QList<int> selection;
    selection.append(1);
    selection.append(0);
    Element *selectedElement ;
    selectedElement = regola->findElementByArray(selection);
    if( NULL != selectedElement ) {
        XmlEditWidget *editor = app.mainWindow()->getEditor();
        editor->setCurrentItem(selectedElement);

        AttributeFilterDialog dialog(parent, element) ;
        dialog.show();
        QTest::sleep();
        dialog.accept();

        AttributeFilter *newFilter = dialog.buildFilter();
        if(NULL != newFilter) {
            app.mainWindow()->setAttributeProfile(newFilter);
            if( !app.mainWindow()->_attributeFilterWidget->isVisible() ) {
                return error(msgTest, );
            }
            app.mainWindow()->setAttributeProfile(NULL);
            if( app.mainWindow()->_attributeFilterWidget->isVisible() ) {
                return error(msgTest, );
            }
        }
        if(!icon.isNull()) {
            return error(msg1, "2");
        }
        attributeFilter.attributeNames().insert("attr2");
        paintInfo->setAttributeFilter(&attributeFilter);
        editor->repaint();
        str = item->text(paintInfo->columnForAttributes);
        if( str.contains("attr1=") ) {
            return error(msg1, "3");
        }
        icon = item->icon(paintInfo->columnForAttributes);
        if(icon.isNull()) {
            return error(msg1, "4");
        }
        paintInfo->setAttributeFilter(NULL);
        editor->repaint();
        str = item->text(paintInfo->columnForAttributes);
        if( !str.contains("attr1=") ) {
            return error(msg1, "5");
        }
        icon = item->icon(paintInfo->columnForAttributes);
        if(!icon.isNull()) {
            return error(msg1, "6");
        }
        return true ;
    } else {
        return error(msg1, "7");
    }
}
*/


bool TestFilterAttributes::tfInsertData( SQLLiteTestAccess &access, TestPData *data1, TestPData *data2 )
{
    QString testName = "insertData";

    bool status = true;
    data1->profile = createProfile(0, "a");
    data1->detail = createDetail("name1", "name2");
    data2->profile = createProfile(0, "b");
    data2->detail = createDetail("name1", "");

    if( ! ( data1->isReady()  && data2->isReady() ) ) {
        status = error(testName, "alloc objects");
    } else {
        DataResult result1;
        access.access->attributeNamesFilterSaveProfile(result1, data1->profile, data1->detail );
        DataResult result2;
        access.access->attributeNamesFilterSaveProfile(result2, data2->profile, data2->detail );
        if( !result1.isOk() ) {
            status = error(testName, QString("insert profile 1: %1").arg(result1.message()));
        }
        if( !result2.isOk() ) {
            status = error(testName, QString("insert profile 2: %1").arg(result2.message()));
        }
    }

    return status ;
}


bool TestFilterAttributes::tfOpenProfileDialog( TestFilterAttributes::FTest &fTest )
{
    fTest.cfgDialog->show();
    return true;
}


bool TestFilterAttributes::profileDialogReadData( TestFilterAttributes::FTest &fTest, const QString &testName )
{
    int listSize = fTest.cfgDialog->ui->profilesList->count() ;
    if( 2 != listSize ) {
        return error(testName, QString("profile list expected 2 found: %1").arg(listSize));
    }
    fTest.cfgDialog->ui->profilesList->setCurrentRow(-1);
    if( fTest.cfgDialog->ui->deleteButton->isEnabled() ) {
        return error(testName, QString("profile list delete enabled without selection"));
    }
    if( fTest.cfgDialog->ui->editButton->isEnabled() ) {
        return error(testName, QString("profile list edit enabled without selection"));
    }
    if( fTest.cfgDialog->ui->activateButton->isEnabled() ) {
        return error(testName, QString("profile list activate enabled without selection"));
    }
    return true;
}


bool TestFilterAttributes::profileDialogSelectOne( TestFilterAttributes::FTest &fTest, const QString &testName, TestPData *data )
{
    fTest.cfgDialog->ui->profilesList->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
    QListWidgetItem *item = fTest.cfgDialog->ui->profilesList->currentItem();
    if( NULL == item ) {
        return error(testName, QString("No selected item"));
    }
    AttrFilterProfile *profile = fTest.cfgDialog->profileFromItem(item);
    if( NULL == profile ) {
        return error(testName, QString("null profile"));
    }
    if( !fTest.cfgDialog->ui->deleteButton->isEnabled() ) {
        return error(testName, QString("profile list delete not enabled with selection"));
    }
    if( !fTest.cfgDialog->ui->editButton->isEnabled() ) {
        return error(testName, QString("profile list edit not enabled with selection"));
    }
    if( !fTest.cfgDialog->ui->activateButton->isEnabled() ) {
        return error(testName, QString("profile list activate not enabled with selection"));
    }
    //QScopedPointer<AttrFilterProfile> p (createProfile(0, "a"));
    bool isEquals = data->profile->compareTo(profile);
    if( !isEquals ) {
        return error(testName, QString("profile list selected not equals to inseted one."));
    }
    return true ;
}

//--------------------------------------------------------------------------------------------------
TestAttributeProfileMgmtDialog::TestAttributeProfileMgmtDialog( UIDelegate *uiDelegate, AttributeFilterManagement *newManagement, QWidget *parent ) : AttributeProfileMgmtDialog( uiDelegate, newManagement, parent )
{
    hasBeenCalled = false ;
    detailHasBeenCalled = false;
    hasBeenAccepted = false;
}

TestAttributeProfileMgmtDialog::~TestAttributeProfileMgmtDialog()
{
}

void TestAttributeProfileMgmtDialog::showDetails(AttrFilterProfile * profile, AttrFilterDetail * detail)
{
    hasBeenCalled = true ;
    AttributeProfileMgmtDialog::showDetails( profile, detail);
}

AttributeFilterDetailDialog *TestAttributeProfileMgmtDialog::getDetailDialog(AttrFilterProfile *profile, AttrFilterDetail *detail)
{
    TestAttributeFilterDetailDialog *dlg = new TestAttributeFilterDetailDialog( this, _uiDelegate, _mgmt, profile, detail);
    dlg->dlgParent = this ;
    return dlg;
}


void TestAttributeProfileMgmtDialog::accept()
{
    hasBeenAccepted = true ;
    AttributeProfileMgmtDialog::accept();
}

TestAttributeFilterDetailDialog::TestAttributeFilterDetailDialog(QWidget *parent, UIDelegate *uiDelegate, AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail) : AttributeFilterDetailDialog(parent, uiDelegate, mgmt, profile, detail)
{
    dlgParent = NULL ;
}

TestAttributeFilterDetailDialog::~TestAttributeFilterDetailDialog()
{
}

int TestAttributeFilterDetailDialog::go()
{
    dlgParent->detailHasBeenCalled = true ;
    return 1;
}

void TestAttributeFilterDetailDialog::testInsertItem(const QString &newItemText)
{
    QListWidgetItem *item = new QListWidgetItem(newItemText);
    if(NULL == item) {
        fatalError(_uiDelegate->msgOutOfMem());
    }
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->namesList->addItem(item);
    accept();
}

void TestAttributeFilterDetailDialog::editItem(const int rowIndex, const QString &newItemText)
{
    QListWidgetItem *item = ui->namesList->item(rowIndex);
    if( NULL != item ) {
        item->setText(newItemText);
    }
}

void TestAttributeFilterDetailDialog::deleteFirstItem()
{
    QListWidgetItem *item = ui->namesList->takeItem(0);
    if( NULL != item ) {
        delete item;
    }
}
//--------------------------------------------------------------------------------------------------------

bool TestFilterAttributes::profileDialogEdit( TestFilterAttributes::FTest &fTest, const QString &testName )
{
    fTest.cfgDialog->hasBeenCalled = false;
    fTest.cfgDialog->detailHasBeenCalled = false;
    fTest.cfgDialog->ui->profilesList->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
    emit fTest.cfgDialog->ui->editButton->click();
    if( !fTest.cfgDialog->hasBeenCalled ) {
        return error(testName, "edit not called");
    }
    if( !fTest.cfgDialog->detailHasBeenCalled ) {
        return error(testName, "edit, detail not called");
    }
    return true ;
}

bool TestFilterAttributes::profileDialogNew( TestFilterAttributes::FTest &fTest, const QString &testName )
{
    fTest.cfgDialog->hasBeenCalled = false;
    fTest.cfgDialog->detailHasBeenCalled = false;
    fTest.cfgDialog->ui->profilesList->setCurrentRow(-1, QItemSelectionModel::Clear);
    emit fTest.cfgDialog->ui->newButton->click();
    if( !fTest.cfgDialog->hasBeenCalled ) {
        return error(testName, "new not called");
    }
    if( !fTest.cfgDialog->detailHasBeenCalled ) {
        return error(testName, "new, detail not called");
    }
    return true;
}

bool TestFilterAttributes::profileDialogDelete( TestFilterAttributes::FTest &fTest, const QString &testName )
{
    fTest.cfgDialog->ui->profilesList->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
    emit fTest.cfgDialog->ui->deleteButton->click();

    int listSize = fTest.cfgDialog->ui->profilesList->count() ;
    if( 1 != listSize ) {
        return error(testName, QString("delete, expected 1 found: %1").arg(listSize));
    }
    return true;
}

bool TestFilterAttributes::profileDialogChoose( TestFilterAttributes::FTest &fTest, const QString &testName )
{
    fTest.cfgDialog->ui->profilesList->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
    emit fTest.cfgDialog->ui->activateButton->click();
    if( !fTest.cfgDialog->hasBeenAccepted ) {
        return error(testName, "select not accepted");
    }
    // check the selected element
    if( fTest.cfgDialog->selectedProfileCode() == -1 ) {
        return error(testName, "invalid selected code");
    }
    return true;
}

bool TestFilterAttributes::closeProfileDialog(TestFilterAttributes::FTest &fTest)
{
    fTest.cfgDialog->close();
    return true ;
}

void TestAttributeFilterDetailDialog::setProfileData(const QString &newName, const QString &newDescr, const bool isWhiteList )
{
    this->ui->txtName->setText(newName);
    this->ui->txtDescription->setText(newDescr);
    this->ui->radioShow->setChecked(isWhiteList);
    this->ui->radioHide->setChecked(!isWhiteList);
}

bool TestFilterAttributes::saveDetail(AttributeFilterManagement *mgmt, AttrFilterProfile *profile, AttrFilterDetail *detail, const QString &newName, const QString &newDescr, const bool isWhiteList )
{
    FakeUIDelegateYes uiDelegate;
    TestAttributeFilterDetailDialog dlg(NULL, &uiDelegate, mgmt, profile, detail);
    // update the data
    dlg.setProfileData(newName, newDescr, isWhiteList );
    dlg.accept();
    return dlg.result() == QDialog::Accepted ;
}

/*
nella funziona successiva la parametrizzo
sostituire con la suxxessiva
bool TestFilterAttributes::testDetailXXXInsert(const QString &testName, SessionDataInterface *access, const bool isInsert )
{
    QString newName("A123");
    AttributeFilterManagement mgmt;
    mgmt.setDataAccess(access);

    TestPData dt(true);
    TestPData dt2(true);

    if( !dt.isReady() || !dt2.isReady()) {
        return error(testName, "alloc objects");
    }
    if( !saveDetail( &mgmt, dt.profile, dt.detail, newName) ) {
        return error(testName, QString("executing save profile"));
    }
    FakeUIDelegateYes uiDelegate;
    TestAttributeFilterDetailDialog dlg( NULL, &uiDelegate, _mgmt, profile, detail);
    dlg.show();
    dlg.testInsertItem(newName);+delete

    QList<AttrFilterProfile*> readList = access->attributeNamesFilterReadProfiles(result2);
    if( !result2.isOk() ) {
        return error(testName, QString("read profile list failed: %1").arg(result2.message()));
    }
    if( readList.size() != 1 ) {
        return error(testName, QString("Data read: expected 1, found %1").arg(readList.size()));
    }

    DataResult resultDetail;
    AttrFilterDetail *readDetail = mgmt.readDetail(resultDetail, dt.profile->id());
    if( !resultDetail.isOk() ) {
        return error(testName, QString("Detail read "));
    }
    bool isEqual = false;
    dt2.detail->addName(newName);
    if( dt2.detail->compareTo(readDetail) ) {
        isEqual = true ;
    }
    if( NULL != readDetail ) {
        delete readDetail ;
    }
    if(!isEqual) {
        return error(testName, QString("detail is different"));
    }
    return true ;
}
*/


bool TestFilterAttributes::parametrizedDetailTest(const QString &testName, bool (TestFilterAttributes::*oper) (TestAttributeFilterDetailDialog *dlg), AttrFilterProfile *expectedProfile, AttrFilterDetail *expectedDetail )
{
    //QString testName = "testDetailEdit" ;
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init access");
    }
    AttributeFilterManagement mgmt;
    mgmt.setDataAccess(access.access);

    TestPData dt(true);

    if( !dt.isReady() ) {
        return error(testName, "alloc objects");
    }
    /**
    if( !saveDetail( &mgmt, dt.profile, dt.detail, expectedProfile->name(), expectedProfile->description(), expectedProfile->isWhiteList()) ) {
        return error(testName, QString("executing save profile"));
    }
    */
    FakeUIDelegateYes uiDelegate;
    TestAttributeFilterDetailDialog dlg( NULL, &uiDelegate, &mgmt, dt.profile, dt.detail);
    if(! (this->*oper)(&dlg) ) {
        return error(testName, QString("oper failed"));
    }

    DataResult result2;
    QList<AttrFilterProfile*> readList = mgmt.readProfiles(result2);
    if( !result2.isOk() ) {
        return error(testName, QString("read profile list failed: %1").arg(result2.message()));
    }
    if( readList.size() != 1 ) {
        return error(testName, QString("Data read: expected 1, found %1").arg(readList.size()));
    }

    AttrFilterProfile *read = readList.at(0);
    bool status = false;
    expectedProfile->setId(read->id());
    if( expectedProfile->compareToBase(read)) {
        status = true ;
    }
    if( NULL != read ) {
        delete read;
    }
    if(!status) {
        return error(testName, QString("Profile not equal to expected"));
    }

    DataResult resultDetail;
    AttrFilterDetail *readDetail = mgmt.readDetail(resultDetail, expectedProfile->id());
    if( !resultDetail.isOk() ) {
        return error(testName, QString("Detail read "));
    }
    bool isEqual = false;
    if( expectedDetail->compareTo(readDetail) ) {
        isEqual = true ;
    }
    if( NULL != readDetail ) {
        delete readDetail ;
    }
    if(!isEqual) {
        return error(testName, QString("detail is different"));
    }
    return true ;
}


bool TestFilterAttributes::testDetailEdit(const QString &testName, SessionDataInterface *access, const bool isInsert )
{
    QString newName("A123");
    AttributeFilterManagement mgmt;
    mgmt.setDataAccess(access);

    TestPData dt(true);

    if( !dt.isReady() ) {
        return error(testName, "alloc objects");
    }
    if( !isInsert ) {
        DataResult result1;
        access->attributeNamesFilterSaveProfile(result1, dt.profile, dt.detail );
        if( !result1.isOk() ) {
            return error(testName, QString("insert profile 1: %1").arg(result1.message()));
        }
    }
    if( !saveDetail( &mgmt, dt.profile, dt.detail, newName, dt.profile->description(), dt.profile->isWhiteList()) ) {
        return error(testName, QString("executing save profile"));
    }
    DataResult result2;
    QList<AttrFilterProfile*> readList = access->attributeNamesFilterReadProfiles(result2);
    if( !result2.isOk() ) {
        return error(testName, QString("read profile list failed: %1").arg(result2.message()));
    }
    if( readList.size() != 1 ) {
        return error(testName, QString("Data read: expected 1, found %1").arg(readList.size()));
    }
    bool status = true ;
    AttrFilterProfile* profileRead = readList.at(0);
    dt.profile->setName(newName);
    // set the data that we do not know
    dt.profile->setId(profileRead->id());
    if( !dt.profile->compareToBase(profileRead) ) {
        status = error(testName, QString("Data read not equals to set data."));
    }
    delete profileRead ;
    return status ;
}

bool TestFilterAttributes::testDetailEdit()
{
    QString testName = "testDetailEdit" ;
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init access");
    }
    return testDetailEdit(testName, access.access, false );
}

bool TestFilterAttributes::testDetailInsert()
{
    QString testName = "testDetailEdit" ;
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init access");
    }
    return testDetailEdit(testName, access.access, true );
}


bool TestFilterAttributes::operDeleteItem(TestAttributeFilterDetailDialog *dlg)
{
    dlg->deleteFirstItem();
    dlg->accept();
    return true;
}


bool TestFilterAttributes::operEditItem(TestAttributeFilterDetailDialog *dlg)
{
    dlg->editItem(0, "xxx");
    dlg->accept();
    return true;
}


bool TestFilterAttributes::testDetailDeleteItem()
{
    QString testName = "testDetailDeleteItem" ;
    TestPData dt(true);

    if( !dt.isReady() ) {
        return error(testName, "alloc objects 0");
    }
    dt.detail->clear();
    dt.detail->addName("name2");
    if( !parametrizedDetailTest(testName, &TestFilterAttributes::operDeleteItem, dt.profile, dt.detail ) ) {
        return false;
    }
    return true;
}

bool TestFilterAttributes::testDetailEditItem()
{
    QString testName = "testDetailEditItem" ;
    TestPData dt(true);

    if( !dt.isReady() ) {
        return error(testName, "alloc objects 0");
    }
    dt.detail->clear();
    dt.detail->addName("name2");
    dt.detail->addName("xxx");
    if( !parametrizedDetailTest(testName, &TestFilterAttributes::operEditItem, dt.profile, dt.detail ) ) {
        return false;
    }
    return true;
}


/********************************************

  TODO


elenco profili scrivendo nel db
elenco profili check#
scegli profilo, controlla nei dati
elenco profili, nuovo salva
edit profili ->salva
elimina profilo, rileggi


***************************************************/




bool TestFilterAttributes::testFunctionality()
{
    QString testName = "testFunctionality";
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init app");
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    app.data()->attributeFilterManagement()->setDataAccess(access.access);
    TestPData p1 (false);
    TestPData p2 (false);
    if(! tfInsertData(access, &p1, &p2)) {
        return false ;
    }
    FakeUIDelegateYes uiDelegate;
    TestAttributeProfileMgmtDialog dialog(&uiDelegate, app.data()->attributeFilterManagement(), app.mainWindow());
    FTest fTest(&dialog, &app);
    if(! tfOpenProfileDialog(fTest)) {
        return error(testName, "open Dialog");
    }

    if(! profileDialogReadData(fTest, testName)) {
        return false;
    }

    if(! profileDialogSelectOne(fTest, testName, &p1)) {
        return false;
    }

    if(! profileDialogEdit(fTest, testName )) {
        return false;
    }

    if(! profileDialogNew(fTest, testName )) {
        return false;
    }
    if(! profileDialogDelete(fTest, testName)) {
        return false ;
    }

    closeProfileDialog(fTest);

    TestAttributeProfileMgmtDialog dialogC(&uiDelegate, app.data()->attributeFilterManagement(), app.mainWindow());
    FTest fTestC(&dialogC, &app);
    if(! tfOpenProfileDialog(fTestC)) {
        return error(testName, "open Dialog C");
    }
    if(! profileDialogChoose( fTestC, testName)) {
        return false;
    }
    closeProfileDialog(fTestC);
/*
    if( ! testDetailEdit() ) {
        return false;
    }

    if( ! testDetailInsert() ) {
        return false;
    }

    // todo: add item, test

    if(!testDetailDeleteItem()) {
        return false;
    }

    if(!testDetailEditItem()) {
        return false;
    }
    */
    return true;
}


bool TestFilterAttributes::testDetailFunctionality()
{
    QString testName = "testDetailFunctionality";
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error(testName, "init app");
    }
    App app;
    app.sessionDBPath = access.dbFileName;
    app.init1();
    app.data()->attributeFilterManagement()->setDataAccess(access.access);
    TestPData p1 (false);
    TestPData p2 (false);
    /*
    if(! tfInsertData(access, &p1, &p2)) {
        return false ;
    }
    */
    FakeUIDelegateYes uiDelegate;
    /*
    TestAttributeProfileMgmtDialog dialog(app.data()->attributeFilterManagement(), &uiDelegate, app.mainWindow());
    FTest fTest(&dialog, &app);
    if(! tfOpenProfileDialog(fTest)) {
        return error(testName, "open Dialog");
    }

    if(! profileDialogReadData(fTest, testName)) {
        return false;
    }

    if(! profileDialogSelectOne(fTest, testName, &p1)) {
        return false;
    }

    if(! profileDialogEdit(fTest, testName )) {
        return false;
    }

    if(! profileDialogNew(fTest, testName )) {
        return false;
    }
    if(! profileDialogDelete(fTest, testName)) {
        return false ;
    }

    closeProfileDialog(fTest);

    TestAttributeProfileMgmtDialog dialogC(app.data()->attributeFilterManagement(), &uiDelegate, app.mainWindow());
    FTest fTestC(&dialogC, &app);
    if(! tfOpenProfileDialog(fTestC)) {
        return error(testName, "open Dialog C");
    }
    if(! profileDialogChoose( fTestC, testName)) {
        return false;
    }
    closeProfileDialog(fTestC);
*/
    if( ! testDetailEdit() ) {
        return false;
    }

    if( ! testDetailInsert() ) {
        return false;
    }

    // todo: add item, test

    if(!testDetailDeleteItem()) {
        return false;
    }

    if(!testDetailEditItem()) {
        return false;
    }

    return true;
}

