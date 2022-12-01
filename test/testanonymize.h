/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2022 by Luca Bellonda and individual contributors  *
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
class ApplicationData;
class OperationStatus;
class AnonStatAlgPattern;
class AnonAlgFreqCount;
class AnonAlgFreqTarget;

#include "modules/anonymize/anonexception.h"
#include "modules/anonymize/anonoperationbatch.h"
#include "modules/anonymize/anonbase.h"
#include "testhelpers/collect.h"

class TestAnonymize : public TestBase, AnonOperationBatchOutputFileProvider
{
    enum Resolution {
        ResolveElement = 0,
        ResolveAttribute = 1,
        ResolveText = 2
    };

    QString _askedOutString;
    QIODevice *_retProvided;

    Collect<AnonProfile> _p;
    Collect<GenericPersistentData> _g;
    Collect<OperationStatus> _o;
    Collect<AnonException> _e;

public:
    virtual QIODevice *outProviderProvide(const QString &filePath);
    virtual void outProviderAutoDelete();
    virtual void outProviderDeleteIO(QIODevice *);
private:
    QDateTime _baseDate;
    bool errorButton(const QString &buttonName );
    bool errorText(const QString &widgetName );
    bool operateButton(QWidget *dlg, const QString &buttonName );
    AnonException *createAEx( const AnonInclusionCriteria::Criteria criteria, const AnonType::Type type, const bool useNs);
    bool compareProfiles(AnonProfile *p1, AnonProfile *p2);
    bool compareProfilesWrongParams(AnonProfile *p1, AnonProfile *p2);
    bool compareExceptions(const int index, AnonException *e1, AnonException *e2);
    AnonProfile *createProfile();
    void fillProfile(GenericPersistentData *o1, const QString &newPayload, const QString &newName="");
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
    bool testUnitAlgStat();
    bool testAnonStatContext();
    bool testAnonUtils();
    bool testAnonStatRandom();
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
    //------
    bool testCodeWithLen1();
    bool testCodeWithLen2();
    bool testCodeWithLen10();
    bool testCodeWithLen20();
    bool testAlgCodeVariableLen(const int len, const QString &fileStart, const QString &fileEnd);
    bool testAlgCodeVariableCodeLen();

    //---------------------
    bool testAnonStatFreqTarget();
    bool testUnitAlgStatScanValue();
    bool testAlgStatScanValueNull();
    bool testAlgStatScanValueEmpty();
    bool testAlgStatScanValueOneWord();
    bool testAlgStatScanValueDoubleWord();
    bool testAlgStatScanValue3Word();
    bool testAlgStatScanValueW1();
    bool testAlgStatScanValueW2();
    bool testAlgStatScanValueW3();
    bool testAlgStatScanValueW4();
    bool testAlgStatScanValueW5();
    bool testAlgStatScanValueW6();
    bool testAlgStatScanValueCase(QStringList input, const int expectedTotal, QList<QPair<int,int>> &expectedItems );
    //
    bool testAnonStatTargetValue();
    bool testStatValueForInputs( const QStringList &inputs, const QString expectedOutput);
    bool testStatValueForInputsCoded( const QStringList &inputs, const QString expectedOutput, const AnonProducer::ESeqType type1, AnonProducer::ESeqType type2);
    //
    bool testIsInInterval(AnonAlgFreqTarget *target, const double input, const bool expected );
    bool testAnonStatForTargetValue(AnonAlgFreqTarget *target);
    //
    bool testUnitAnonStatAlgWordChain();
    bool testAnonStatWordChain3Word();
    bool testAnonStatWordChainDoubleWord();
    bool testAnonStatWordChainOneWord();
    bool testAnonStatWordChainEmpty();
    bool testAlgStatWordChainCase(QList<QStringList> input, QStringList &expectedPatterns );
    bool testAlgStatScanWordChainCase(QStringList input, const int expectedTotal, QList<QPair<int,int>> &expectedItems );
    //
    bool testUnitAnonStatAlgWord();
    bool testAlgStatScanWordCase(QStringList input, const int expectedTotal, QList<QPair<int,int>> &expectedItems, QStringList expectedPatterns );
    bool testUnitAnonStatAlgWordErrors();
    bool testUnitAnonStatAlgWordHitPatterns1();
    bool testUnitAnonStatAlgWordHitPatternsNsame();
    bool testUnitAnonStatAlgWordHitPatternsN();
    //
    bool testUnitAnonStatAlgPattern();
    bool testUnitAnonStatAlgPatterns();
    bool testUnitAnonStatAlgSinglePattern(const QString &aPattern, const QString &expected, AnonProducer::ESeqType expectedType);
    bool checkPatternCount(AnonStatAlgPattern *anonStatAlgPattern, const int expected);
    bool testAnonStatAlgPattern();
    bool testAnonStatAlgPatternLoop();
    bool testUnitAnonStatAlgPatternsBase();
    //
    bool testUnitAlgStatValue();
    //
    bool testAnonStatMean();
    bool testAnonStatRandomChoice();
    bool testAnonStatPrepare();
    //
    bool checkBaseStatIntervals(AnonAlgFreqCount *accumulator, const QList<AnonAlgFreqTarget*> &expectedTargets );
    bool checkStatIntervals(const QStringList &inputs, const QList<QPair<double, double> > &expected );
    bool checkStatOneRandomChoice(AnonAlgFreqCount *freqCount, const double randomChoice, const int expectedId);
    bool checkStatRandomChoice(const QStringList &inputs, const double randomChoice, const int expectedId);
    //
    bool testBatchBaseStat();
    bool testBatchBaseStatSkeleton(const QString &code, const QString &sourceFilePath, AnonContext *context, const bool checkCodes, const AnonProducer::ESeqType type);
    bool testBatchBaseStatAFile(const QString &code, const QString &dataFile);
    bool testBatchBaseStatAFile(const QString &code, const QString &dataFile, const bool checkCodes, const AnonProducer::ESeqType type);
    //
    bool testAnonStatChars();
    bool testAnonStatAlgVaria();
    bool testStatValueBlankValue();
    bool testStatErrorProducer();
    bool testAnonUtilsOne(const int code, const AnonProducer::ESeqType expected, const bool expectedRetValue);
    bool cfrStatMem(QBuffer *resultData, const QString &step, const QString &fileResult, const bool checkCodes, const AnonProducer::ESeqType expectedType);

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
    bool innerBatchCommandLine(const QString &id, const QString &newFileInputPath, const QString &newProfileName, const QString &newFileOutputPath,
                                              const bool expectedResult, const QString &compareOutput);
    bool insProfile(ApplicationData *data, const QString &name);
public:
    TestAnonymize();
    ~TestAnonymize();

    bool testBase();
    bool testUnit();
    bool testExc();
    bool testFast();
    bool testBatch();
    bool testBatchCommandLine();
};

#endif // TESTANONYMIZE_H
