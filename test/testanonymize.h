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


#ifndef TESTANONYMIZE_H
#define TESTANONYMIZE_H

#include <QDateTime>
#include <QBuffer>
#include "testbase.h"

class AnonAlg;
class AnonContext;
class AnonProfile;
class GenericPersistentData;
class AnonOperationBatch;
class App;

#include "modules/anonymize/anonexception.h"

class TestAnonymize : public TestBase
{
    enum Resolution {
        ResolveElement = 0,
        ResolveAttribute = 1,
        ResolveText = 2
    };

    QDateTime _baseDate;
    bool errorButton(const QString &buttonName );
    bool errorText(const QString &widgetName );
    bool operateButton(QWidget *dlg, const QString &buttonName );
    AnonException *createAEx( const AnonInclusionCriteria::Criteria criteria, const AnonType::Type type, const bool useNs);
    bool compareProfiles(AnonProfile *p1, AnonProfile *p2);
    bool compareProfilesWrongParams(AnonProfile *p1, AnonProfile *p2);
    bool compareExceptions(const int index, AnonException *e1, AnonException *e2);
    AnonProfile *createProfile();
    void fillProfile(GenericPersistentData *o1, const QString &newPayload);
    bool testImportExceptions();
    bool testExportExceptions();

    //--
    bool testUnitBase(const QString &theTestName, const QString &inputText, const QString &expected, AnonAlg* alg);
    bool cfr(Regola *regola, const QString &step, const QString &fileResult);
    bool cfrMem(QBuffer *resultData, const QString &step, const QString &fileResult);
    bool testSkeleton(const QString &fileStart, const QString &fileResult, AnonAlg *alg);
    bool testSkeletonBase(const QString &fileStart, const QString &fileResult, AnonContext *context);
    bool testAlgAllProdFixed();
    bool testAlgAllProdSeq();
    bool testUnitAlgAllProdFixed();
    bool testUnitAlgAllProdSeq();
    bool testUnitAlgCodeProdFixed();
    bool testUnitAlgCodeProdSeq() ;
    bool testAlgCodeProdFixed();
    bool testAlgCodeProdSeq();
    //------
    bool testExcBase();
    bool testExceptions();
    AnonContext *newStdContext();
    AnonException *newException(const QString &path, AnonInclusionCriteria::Criteria crit);
    void addException(AnonContext *context, const QString &path, AnonInclusionCriteria::Criteria crit, const bool processNamespaces=false);
    //---
    bool testExceptionElement();
    bool testExceptionElementRecursive();
    bool testExceptionElementIncludeRecursive();
    bool testExceptionElementInclude();
    bool testBaseExceptionElement();
    bool testBaseExceptionText();
    bool testInsertException();
    bool testBaseExceptionAttributes();
    bool testSaveAndReadProfile();
    bool testProfileParams();
    bool compareProfilesParams(AnonProfile *profile);
    bool compareProfilesParamsInverse(AnonProfile *profile);
    //---
    bool testBatchBase();
    bool testBatchBaseSkeleton(const QString &sourceFilePath, const QString &fileResult, AnonContext *context);
    //--
    bool testUnitPath();
    bool testUnitPathBase(const QString &testToken, const QString &fileStart, const QString &spec, QList<int> selection, const QString &expected, const Resolution resolution);

public:
    bool testExceptionTextInclude();
private:
    bool testExceptionTextExclude();
    bool testExceptionTextIncludeInline();
    bool testExceptionTextExcludeInline();
    bool testExceptionAttributeInclude();
    bool testExceptionAttributeExclude();

    //---
    bool errorCriteria(const int found, const int expected );
    //---
    bool testException(const QString &test, const bool isElement, const QString &fileStart, const QString &path, const bool include,
                                      const bool recursive, const bool isDefaultAlg, const QString &value );
    bool testExceptionA(const QString &test, const QString &fileStart, const QString &path1, const QString &path2,
                                       const bool include,
                                      const bool isDefaultAlg, const QString &value );
    bool checkException(AnonException *e, const QString &path, const bool include,
                                          const bool recursive, const bool isDefaultAlg, const QString &value );

    bool testBaseProfile();
    bool checkgetExc(QList<int> &selection, App *app, const bool expectedNamespace, const QString &expectedPath , const bool isElement);
    bool checkgetAttrExc(QList<int> &selection, App *app, const bool expectedNamespace, const QString &expectedPath);
public:
    TestAnonymize();
    ~TestAnonymize();

    bool testBase();
    bool testUnit();
    bool testExc();
    bool testFast();
    bool testBatch();
};

#endif // TESTANONYMIZE_H
