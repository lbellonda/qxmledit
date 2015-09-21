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

#include "testanonymize.h"
#include "app.h"
#include "comparexml.h"
#include "testsqlaccess.h"
#include "modules/anonymize/anonbase.h"
#include "modules/anonymize/anonallalg.h"
#include "modules/anonymize/anonfixedalg.h"
#include "modules/anonymize/anonseqproducer.h"
#include "modules/anonymize/anoncodealg.h"
#include "modules/anonymize/anoncontext.h"
#include "modules/anonymize/anonaddexcelementdialog.h"
#include "modules/anonymize/anonattrexcdialog.h"
#include "modules/anonymize/anonprofile.h"
#include "modules/anonymize/anonexception.h"
#include "modules/anonymize/anonprofilemanager.h"
#include "modules/anonymize/anonoperationbatch.h"
#include "modules/anonymize/anonnullalg.h"
#include "modules/xml/elmpath.h"
#include "modules/anonymize/anonadvdialog.h"
#include "modules/anonymize/anonsettingwidget.h"
#include "modules/anonymize/anonattrexcdialog.h"

#define FILE_START_ALGALL_SEQALL            "../test/data/anon/algall_seqall_start.xml"
#define FILE_START_ALGALL_SEQALL2           "../test/data/anon/algall_seqall_start2.xml"
#define FILE_START_ALGALL_SEQALL_END        "../test/data/anon/algall_seqall_end.xml"
#define FILE_START_ALGALL_SEQCHG_END        "../test/data/anon/algall_seqchg_end.xml"
#define FILE_START_ALGCODE                  "../test/data/anon/algcode_start.xml"
#define FILE_START_ALGCODE_PRODSEQ_END      "../test/data/anon/algcode_prodseq_end.xml"
#define FILE_START_ALGCODE_PRODFIX_END      "../test/data/anon/algcode_prodfixed_end.xml"
#define FILE_START_ALGCODE_FIXED            "../test/data/anon/algcode_prodfixed_start.xml"

//-------------------------------

#define FILE_START_ALGSTART         "../test/data/anon/ctx_start.xml"
#define FILE_START_ALGEND               "../test/data/anon/ctx_end.xml"
#define FILE_START_ALGEND_EXCLUDE       "../test/data/anon/ctx_end_exclude.xml"
#define FILE_START_ALGEND_EXCLUDEREC    "../test/data/anon/ctx_end_exclude_rec.xml"
#define FILE_END_INCLUDE                 "../test/data/anon/ctx_end_include.xml"
#define FILE_END_INCLUDE_RECURSIVE      "../test/data/anon/ctx_end_include_rec.xml"
#define FILE_END_TEXT_INCLUDE            "../test/data/anon/ctx_end_text_include.xml"
#define FILE_END_TEXT_EXCLUDE            "../test/data/anon/ctx_end_text_exclude.xml"
#define FILE_END_TEXT_INCLUDE_INLINE            "../test/data/anon/ctx_end_text_include_inline.xml"
#define FILE_END_TEXT_EXCLUDE_INLINE            "../test/data/anon/ctx_end_text_exclude_inline.xml"
#define FILE_END_ATTR_INCLUDE                   "../test/data/anon/ctx_end_attr_include.xml"
#define FILE_END_ATTR_EXCLUDE                   "../test/data/anon/ctx_end_attr_exclude.xml"
//-------------------------------
#define EXC_EL_FILE                     "../test/data/anon/exc_el_file.xml"
//-------------------------------
#define PROFILE_XML                     "../test/data/anon/profilexml.xml"
//----
#define ANON_BATCH_COPY1                "../test/data/anon/batch.copy.xml"


TestAnonymize::TestAnonymize()
{
}

TestAnonymize::~TestAnonymize()
{
}


bool TestAnonymize::testFast()
{
    _testName = "testFast";

    if(!testInsertException()){
        return false;
    }

    if(!testBaseProfile()) {
        return false;
    }
    return true ;
}

bool TestAnonymize::testUnit()
{
    _testName = "testUnit";

    if(!testUnitAlgAllProdFixed()) {
        return false;
    }
    if(!testUnitAlgAllProdSeq()) {
        return false;
    }

    if(!testUnitAlgCodeProdFixed()) {
        return false;
    }
    if(!testUnitAlgCodeProdSeq()) {
        return false;
    }

    if(!testAlgAllProdFixed()) {
        return false;
    }
    if(!testAlgAllProdSeq()) {
        return false;
    }
    if(!testAlgCodeProdFixed()) {
        return false;
    }
    if(!testAlgCodeProdSeq()) {
        return false;
    }
    return true ;
}

//--------------------------------------------------

bool TestAnonymize::testSkeleton(const QString &fileStart, const QString &fileResult, AnonAlg *alg)
{
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    app.mainWindow()->onAnonymize(alg);

    if(!cfr(regola, "operation", fileResult)){
        return false;
    }
    regola->undo();
    if(!cfr(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult)){
        return false;
    }
    return true ;
}

//--------------------------------------------------

bool TestAnonymize::cfr(Regola *regola, const QString &step, const QString &fileResult)
{
    QByteArray resultData = regola->writeMemory();
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    QBuffer outputData(&resultData);
    if(!compare.loadFileIntoDocument(&outputData, document2)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(compare.errorString()));
    }
    return true ;
}

bool TestAnonymize::cfrMem(QBuffer *resultData, const QString &step, const QString &fileResult)
{
    QDomDocument document1;
    QDomDocument document2;
    CompareXML compare;
    if(!compare.loadFileIntoDocument(fileResult, document1)) {
        return error(QString("step: %1, load file result %2").arg(step).arg(fileResult));
    }
    if(!compare.loadFileIntoDocument(resultData, document2)) {
        return error(QString("step %1 load modified data").arg(step));
    }
    bool result = compare.compareDomDocuments(document1, document2);
    if( !result ) {
        compare.dumpErrorCause();
        return error(QString("Step: %1 comparing file with doc: %2").arg(step).arg(compare.errorString()));
    }
    return true ;
}

//--------------------------------------------------

bool TestAnonymize::testAlgAllProdFixed()
{
    _testName = "testAlgAllProdFixed";

    AnonFixedProducer *producer = new AnonFixedProducer();
    AnonAllAlg allAlg(false, producer);
    if( !testSkeleton(FILE_START_ALGALL_SEQALL, FILE_START_ALGALL_SEQALL_END, &allAlg) ) {
        return false ;
    }
    return true;
}

//--------------------------------------------------

bool TestAnonymize::testAlgAllProdSeq()
{
    _testName = "testAlgAllProdSeq";

    AnonSeqProducer *producer = new AnonSeqProducer();
    AnonAllAlg allAlg(false, producer);
    if( !testSkeleton(FILE_START_ALGALL_SEQALL2, FILE_START_ALGALL_SEQCHG_END, &allAlg) ) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgCodeProdFixed()
{
    _testName = "testAlgCodeProdFixed";

    AnonFixedProducer *producer = new AnonFixedProducer();
    AnonCodeAlg allAlg( false, producer);
    if( !testSkeleton(FILE_START_ALGCODE_FIXED, FILE_START_ALGCODE_PRODFIX_END, &allAlg) ) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testAlgCodeProdSeq()
{
    _testName = "testAlgCodeProdSeq";

    AnonSeqProducer *producer = new AnonSeqProducer();
    AnonCodeAlg allAlg( false, producer);
    if( !testSkeleton(FILE_START_ALGCODE, FILE_START_ALGCODE_PRODSEQ_END, &allAlg) ) {
        return false ;
    }
    return true;
}

//--------------------------------------------------

bool TestAnonymize::testUnitBase(const QString &theTestName, const QString &inputText, const QString &expected, AnonAlg* alg)
{
    _testName = theTestName ;
    QString result = alg->processText(inputText);
    if( result != expected ) {
        return error(QString("Unit test, expected '%1', found '%2'.").arg(expected).arg(result));
    }
    return true ;
}

//--------------------------------------------------

bool TestAnonymize::testUnitAlgAllProdFixed()
{
    QString testName = "testUnitAlgAllProdFixed";

    AnonFixedProducer *producer = new AnonFixedProducer();
    AnonAllAlg alg(false, producer);
    if( !testUnitBase(testName + "/1", "", "", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/2", " ", " ", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/3", "a", "x", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/4", "A", "X", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/5", "0", "1", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/6", "abcDeF123 bsbs 5345354 a bBb/2 sd-/sd,A.d", "xxxXxX111 xxxx 1111111 x xXx/1 xx-/xx,X.x", &alg) ) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAlgAllProdSeq()
{
    QString testName = "testUnitAlgAllProdSeq";

    AnonSeqProducer *producer = new AnonSeqProducer();
    AnonAllAlg alg( false, producer);
    if( !testUnitBase(testName + "/1", "", "", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/2", " ", " ", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/3", "f", "a", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/4", "A", "B", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/5", "1", "0", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/6", "abcDeF123 bsbs 5345354 a bBb/2 sd-/sd,A.drfted", "cdeFgH123 ijkl 4567890 m nOp/1 qr-/st,U.vwxyza", &alg) ) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAlgCodeProdFixed() {
    QString testName = "testUnitAlgCodeProdFixed";
    AnonFixedProducer *producer = new AnonFixedProducer();
    AnonCodeAlg alg( false, producer);

    if( !testUnitBase(testName + "/1", "", "", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/2", " ", " ", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/3", "a", "a", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/4", "A", "A", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/5", "ab", "ab", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/6", "AB", "AB", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/7", "abc", "abc", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/8", "ABc", "ABc", &alg) ) {
        return false ;
    }


    if( !testUnitBase(testName + "/9", "abcD", "xxxX", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/10", "ABCd", "XXXx", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/11", "abcdE", "xxxxX", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/12", "ABcDE", "XXxXX", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/13", "0", "0", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/14", "01", "01", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/15", "012", "012", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/16", "0123", "1111", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/17", "04/,.", "04/,.", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/18", "0456/,.", "1111/,.", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/19", "abc./:", "abc./:", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/10", "abcDeF123 bsbs 5345354 a bBb/2 sd-/sd,A.d", "xxxXxX111 xxxx 1111111 x xXx/1 xx-/xx,X.x", &alg) ) {
        return false ;
    }
    return true;
}

bool TestAnonymize::testUnitAlgCodeProdSeq()
{
    QString testName = "testUnitAlgCodeProdSeq";
    AnonSeqProducer *producer = new AnonSeqProducer();
    AnonCodeAlg alg( false, producer);

    if( !testUnitBase(testName + "/1", "", "", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/2", " ", " ", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/3", "a", "a", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/4", "A", "A", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/5", "ab", "ab", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/6", "AB", "AB", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/7", "abc", "abc", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/8", "ABc", "ABc", &alg) ) {
        return false ;
    }


    if( !testUnitBase(testName + "/9", "rstD", "abcD", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/10", "ABCd", "EFGh", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/11", "abcdE", "ijklM", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/12", "ABcDE", "NOpQR", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/13", "2", "2", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/14", "21", "21", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/15", "210", "210", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/16", "3210", "0123", &alg) ) {
        return false ;
    }

    if( !testUnitBase(testName + "/17", "04/,.", "04/,.", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/18", "09874/,.", "45678/,.", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/19", "abc./:", "abc./:", &alg) ) {
        return false ;
    }
    if( !testUnitBase(testName + "/10", "abcDeF123 bsbs 5345354 a bBb/2 sd-/sd,A.d", "stuVwX901 yzab 2345678 c dEf/9 gh-/ij,K.l", &alg) ) {
        return false ;
    }
    return true;
}

//----------------------------------------

bool TestAnonymize::testSkeletonBase(const QString &fileStart, const QString &fileResult, AnonContext *context)
{
    //----- first part: editor
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    app.mainWindow()->onAnonymize(context);

    if(!cfr(regola, "operation", fileResult)){
        return false;
    }
    regola->undo();
    if(!cfr(regola, "undo", fileStart)){
        return false;
    }
    regola->redo();
    if(!cfr(regola, "redo", fileResult)){
        return false;
    }
    //----- second part: batch
    return testBatchBaseSkeleton(fileStart, fileResult, context);
}

bool TestAnonymize::testBatchBaseSkeleton(const QString &sourceFilePath, const QString &fileResult, AnonContext *context)
{
    //----- first part: editor
    AnonOperationBatch tester;
    bool isError = false;
    QByteArray resultData;
    QBuffer buffer(&resultData);
    QFile sourceFile(sourceFilePath);
    if(!buffer.open(QIODevice::ReadWrite)) {
        return error(QString("Batch: unable to open out buffer"));
    }
    if( sourceFile.open(QFile::ReadOnly) )
    {
        const AnonOperationResult *res ;
        res = tester.execute(&sourceFile, &buffer, context);
        if( NULL != res ) {
            if( res->isError() ) {
                error(QString("Batch: error writing was: '%1' '%2'").arg(res->code()).arg(res->message()));
            }
        } else {
            error(QString("Batch: returned null result"));
        }
        buffer.close();
        sourceFile.close();
    } else {
        return error(QString("Batch: unable to open file: '%1'").arg(sourceFilePath));
    }
    if( isError ) {
        return false;
    }
    if(!cfrMem(&buffer, "cmp", fileResult)){
        return false;
    }
    return true;
}

//------------------

bool TestAnonymize::testExc()
{
    _testName = "testExc";

    if(!testExcBase()) {
        return false;
    }
    if(!testExceptions()) {
        return false;
    }
    if(!testImportExceptions()) {
        return false;
    }
    if(!testExportExceptions()) {
        return false;
    }
    return true;
}

bool TestAnonymize::testExceptions()
{
    _testName = "testExceptions";

    if(!testExceptionTextInclude()) {
        return false;
    }
    if(!testExceptionTextExclude()) {
        return false;
    }

    if(!testExceptionTextIncludeInline()) {
        return false;
    }
    if(!testExceptionTextExcludeInline()) {
        return false;
    }

    if(!testExceptionAttributeInclude()) {
        return false;
    }
    if(!testExceptionAttributeExclude()) {
        return false;
    }
    if(!testExceptionElement()) {
        return false;
    }
    if(!testExceptionElementRecursive()) {
        return false;
    }
    if(!testExceptionElementInclude()) {
        return false;
    }
    if(!testExceptionElementIncludeRecursive()) {
        return false;
    }

    return true ;
}

AnonContext *TestAnonymize::newStdContext()
{
    AnonymizeParameters p(AnonymizeParameters::AllText, true);
    AnonContext *context = new AnonContext (NULL, "");
    context->setAlg(&p);
    return context;
}

AnonException *TestAnonymize::newException(const QString &path, AnonInclusionCriteria::Criteria crit)
{
    AnonException *exc = new AnonException();
    exc->setCriteria(crit);
    exc->setPath(path);
    return exc;
}

void TestAnonymize::addException(AnonContext *context, const QString &path, AnonInclusionCriteria::Criteria crit, const bool processNamespaces)
{
    AnonException *exc = newException(path, crit);
    exc->setUseNamespace(processNamespaces);
    context->addException(exc);
}

bool TestAnonymize::testExcBase()
{
    _testName = "testExcBase";

    AnonContext *context = newStdContext();
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_START_ALGEND, context) ;
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionElement()
{
    _testName = "testExceptionElement";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1", AnonInclusionCriteria::Exclude );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_START_ALGEND_EXCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1", AnonInclusionCriteria::Exclude, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_START_ALGEND_EXCLUDE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionElementRecursive()
{
    _testName = "testExceptionElementRecursive";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1", AnonInclusionCriteria::ExcludeWithChildren );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_START_ALGEND_EXCLUDEREC, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1", AnonInclusionCriteria::ExcludeWithChildren, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_START_ALGEND_EXCLUDEREC, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionElementInclude()
{
    _testName = "testExceptionElementInclude";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1", AnonInclusionCriteria::ExcludeWithChildren );
    addException(context, "/a/b1/c1/d1/e1", AnonInclusionCriteria::Include );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_INCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1", AnonInclusionCriteria::ExcludeWithChildren, true );
        addException(context, "/a/b1/{anamespace}c1/{anamespace}d1/{anamespace}e1", AnonInclusionCriteria::Include, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_INCLUDE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionElementIncludeRecursive()
{
    _testName = "testExceptionElementIncludeRecursive";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1", AnonInclusionCriteria::ExcludeWithChildren );
    addException(context, "/a/b1/c1/d1/e1", AnonInclusionCriteria::IncludeWithChildren );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_INCLUDE_RECURSIVE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1", AnonInclusionCriteria::ExcludeWithChildren, true );
        addException(context, "/a/b1/{anamespace}c1/{anamespace}d1/{anamespace}e1", AnonInclusionCriteria::IncludeWithChildren, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_INCLUDE_RECURSIVE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionTextInclude()
{
    _testName = "testExceptionTextInclude";

    AnonContext *context = newStdContext();
    addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren );
    addException( context, "/a/b1/c1/d1/e1/text()", AnonInclusionCriteria::Include );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_INCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren, true );
        addException( context, "/a/b1/{anamespace}c1/{anamespace}d1/{anamespace}e1/text()", AnonInclusionCriteria::Include, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_INCLUDE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionTextExclude()
{
    _testName = "testExceptionTextExclude";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1/d1/e1/text()", AnonInclusionCriteria::Exclude );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_EXCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1/{anamespace}d1/{anamespace}e1/text()", AnonInclusionCriteria::Exclude, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_EXCLUDE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionTextIncludeInline()
{
    _testName = "testExceptionTextIncludeInline";

    AnonContext *context = newStdContext();
    addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren );
    addException( context, "/a/b1/c1/text()", AnonInclusionCriteria::Include );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_INCLUDE_INLINE, context) ;
    if( result ) {
        context->removeExceptions();
        addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren, true );
        addException( context, "/a/b1/{anamespace}c1/text()", AnonInclusionCriteria::Include, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_INCLUDE_INLINE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionTextExcludeInline()
{
    _testName = "testExceptionTextExcludeInline";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1/text()", AnonInclusionCriteria::Exclude );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_EXCLUDE_INLINE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1/text()", AnonInclusionCriteria::Exclude, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_TEXT_EXCLUDE_INLINE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionAttributeInclude()
{
    _testName = "testExceptionAttributeInclude";

    AnonContext *context = newStdContext();
    addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren );
    addException( context, "/a/b1/c1/@a", AnonInclusionCriteria::Include );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_ATTR_INCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException(context, "/a", AnonInclusionCriteria::ExcludeWithChildren, true );
        addException( context, "/a/b1/{anamespace}c1/@a", AnonInclusionCriteria::Include, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_ATTR_INCLUDE, context) ;
    }
    delete context;
    return result ;
}

bool TestAnonymize::testExceptionAttributeExclude()
{
    _testName = "testExceptionAttributeExclude";

    AnonContext *context = newStdContext();
    addException( context, "/a/b1/c1/@a", AnonInclusionCriteria::Exclude );
    bool result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_ATTR_EXCLUDE, context) ;
    if( result ) {
        context->removeExceptions();
        addException( context, "/a/b1/{anamespace}c1/@a", AnonInclusionCriteria::Exclude, true );
        result = testSkeletonBase(FILE_START_ALGSTART, FILE_END_ATTR_EXCLUDE, context) ;
    }
    delete context;
    return result ;
}

//-----------------------------------------------
bool TestAnonymize::testBase()
{
    _testName = "testBase";

    if(!testUnitPath()) {
        return false;
    }

    if(!testBaseProfile()) {
        return false;
    }

    if(!testProfileParams()) {
        return false;
    }

    if(!testSaveAndReadProfile()) {
        return false;
    }

    if(!testBaseExceptionElement()) {
        return false;
    }
    if(!testBaseExceptionAttributes()) {
        return false;
    }
    if(!testBaseExceptionText()) {
        return false;
    } 
    if(!testInsertException()) {
        return false;
    }
    return true;
}

bool TestAnonymize::errorButton(const QString &buttonName )
{
    return error(QString("Unable to find button: %1").arg(buttonName));
}

bool TestAnonymize::errorText(const QString &widgetName )
{
    return error(QString("Unable to find text widget: %1").arg(widgetName));
}

bool TestAnonymize::operateButton(QWidget *dlg, const QString &buttonName )
{
    if(!clickButton(dlg, buttonName)) {
        return errorButton(buttonName);
    }
    return true;
}

bool TestAnonymize::checkException(AnonException *e, const QString &path, const bool include,
                                      const bool recursive, const bool isDefaultAlg, const QString &value )
{
    if( path != e->path() ) {
        return error(QString("Path: expecting '%1', found '%2'").arg(path).arg(e->path()));
    }
    if( include && recursive ) {
        if(!errorCriteria( e->criteria(), AnonInclusionCriteria::IncludeWithChildren) ) {
            return false;
        }
    } else if( include && !recursive ) {
        if(!errorCriteria( e->criteria(), AnonInclusionCriteria::Include ) ) {
            return false;
        }
    } else if( !include && recursive ) {
        if(!errorCriteria( e->criteria(), AnonInclusionCriteria::ExcludeWithChildren ) ) {
            return false;
        }
    } else if( !include && !recursive ) {
        if(!errorCriteria( e->criteria(), AnonInclusionCriteria::Exclude ) ) {
            return false;
        }
    }
    if(include) {
        if( isDefaultAlg ) {
            if(e->anonType() != AnonType::UseDefault) {
                return error(QString("Type: expecting AnonType::UseDefault, found '%2'").arg(e->anonType()));
            }
        } else {
            if(e->anonType() != AnonType::FixedValue) {
                return error(QString("Type: expecting AnonType::FixedValue, found '%2'").arg(e->anonType()));
            }
            if( e->fixedValue() != value ) {
                return error(QString("Type: expecting '%1', found '%2'").arg(e->fixedValue()).arg(value));
            }
        }
    }
    //--------------
    return true;
}


bool TestAnonymize::testException(const QString &test, const bool isElement, const QString &fileStart, const QString &path, const bool include,
                                  const bool recursive, const bool isDefaultAlg, const QString &value )
{
    _testName = test ;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *root = regola->root();
    if( NULL == root ) {
        return error("No root");
    }
    app.mainWindow()->getEditor()->setCurrentItem(root);
    AnonAddExcElementDialog dlg( root, isElement, app.mainWindow() );
    if(include) {
        if(!operateButton(&dlg, "include")) {
            return false;
        }
    } else {
        if(!operateButton(&dlg, "exclude")) {
            return false;
        }
    }
    if(recursive) {
        if(!operateButton(&dlg, "chkRecursive")) {
            return false;
        }
    }
    //------
    if(include) {
        if(isDefaultAlg) {
            if(!operateButton(&dlg, "methodDefault")) {
                return false;
            }
        } else {
            if(!operateButton(&dlg, "methodFixed")) {
                return false;
            }
        }
        if(!setTextWidget(&dlg, "FixedValue", value)) {
            return errorText("FixedValue");
        }
    }
    AnonException *e = dlg.getException();
    if( !checkException(e, path, include, recursive, isDefaultAlg, value ) ) {
        return false;
    }
    return true ;
}

bool TestAnonymize::testExceptionA(const QString &test, const QString &fileStart, const QString &path1, const QString &path2,
                                   const bool include,
                                  const bool isDefaultAlg, const QString &value )
{
    _testName = test ;
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *root = regola->root();
    if( NULL == root ) {
        return error("No root");
    }
    app.mainWindow()->getEditor()->setCurrentItem(root);
    AnonAttrExcDialog dlg( root, app.mainWindow() );
    if(include) {
        if(!operateButton(&dlg, "include")) {
            return false;
        }
    } else {
        if(!operateButton(&dlg, "exclude")) {
            return false;
        }
    }

    //------
    if( include ) {
        if(isDefaultAlg) {
            if(!operateButton(&dlg, "methodDefault")) {
                return false;
            }
        } else {
            if(!operateButton(&dlg, "methodFixed")) {
                return false;
            }
        }
        if(!setTextWidget(&dlg, "FixedValue", value)) {
            return errorText("FixedValue");
        }
    }
    QMap<QString,QString> sortedInputMap;
    sortedInputMap[path1] = path1 ;
    sortedInputMap[path2] = path2 ;
    QList<AnonException*> eList = dlg.getExceptions();
    if( eList.size() != 2 ) {
        return error(QString("Exceptions, expected 2, found:%1").arg(eList.size()));
    }
    AnonException* excs[2];
    if( eList.at(0)->path() < eList.at(1)->path()) {
        excs[0] = eList.at(0);
        excs[1] = eList.at(1);
    } else {
        excs[0] = eList.at(1);
        excs[1] = eList.at(0);
    }
    int i = 0;
    foreach( QString key, sortedInputMap.keys() ) {
        if( !checkException(excs[i], key, include, false, isDefaultAlg, value ) ) {
            return false;
        }
        i++ ;
    }
    return true ;
}

bool TestAnonymize::errorCriteria(const int found, const int expected )
{
    if( expected != found ) {
        return error(QString("Criteria: expecting %1, found %2").arg(expected).arg(found));
    }
    return true;
}

bool TestAnonymize::testBaseExceptionElement()
{
    if(!testException("tbe1", true, EXC_EL_FILE, "/root", false, false, false, "a" ) ) {
        return false;
    }
    if(!testException("tbe2", true, EXC_EL_FILE, "/root", true, false, false, "a" ) ) {
        return false;
    }
    if(!testException("tbe3", true, EXC_EL_FILE, "/root", false, true, false, "a" ) ) {
        return false;
    }
    if(!testException("tbe4", true, EXC_EL_FILE, "/root", false, false, true, "a" ) ) {
        return false;
    }
    if(!testException("tbe5", true, EXC_EL_FILE, "/root", true, true, false, "a" ) ) {
        return false;
    }
    if(!testException("tbe6", true, EXC_EL_FILE, "/root", true, false, true, "a" ) ) {
        return false;
    }
    if(!testException("tbe7", true, EXC_EL_FILE, "/root", false, true, true, "a" ) ) {
        return false;
    }
    if(!testException("tbe8", true, EXC_EL_FILE, "/root", true, true, true, "a" ) ) {
        return false;
    }
    return true;
}

bool TestAnonymize::testBaseExceptionText()
{
    if(!testException("tbt1", false, EXC_EL_FILE, "/root/text()", false, false, false, "" ) ) {
        return false;
    }
    if(!testException("tbt2", false, EXC_EL_FILE, "/root/text()", true, false, false, "a" ) ) {
        return false;
    }
    if(!testException("tbt3", false, EXC_EL_FILE, "/root/text()", true, false, true, "a" ) ) {
        return false;
    }
    /*TODO: not used if(!testException("tbt4", false, EXC_EL_FILE, "/root/text()", false, false, true, "a" ) ) {
        return false;
    }
    if(!testException("tbt5", false, EXC_EL_FILE, "/root/text()", true, false, false, "a" ) ) {
        return false;
    }
    if(!testException("tbt6", false, EXC_EL_FILE, "/root/text()", true, false, true, "a" ) ) {
        return false;
    }
    if(!testException("tbt7", false, EXC_EL_FILE, "/root/text()", false, false, true, "a" ) ) {
        return false;
    }
    if(!testException("tbt8", false, EXC_EL_FILE, "/root/text()", true, false, true, "a" ) ) {
        return false;
    }*/
    return true;
}

bool TestAnonymize::testBaseExceptionAttributes()
{
    if(!testExceptionA("tba1", EXC_EL_FILE, "/root/@a", "/root/@b", false, false, "a" ) ) {
        return false;
    }
    if(!testExceptionA("tba3", EXC_EL_FILE, "/root/@a", "/root/@b", true, false, "a" ) ) {
        return false;
    }
    if(!testExceptionA("tba4", EXC_EL_FILE, "/root/@a", "/root/@b", false, true, "a" ) ) {
        return false;
    }
    if(!testExceptionA("tba7", EXC_EL_FILE, "/root/@a", "/root/@b", true, true, "a" ) ) {
        return false;
    }
    return true;
}

//---

AnonException *TestAnonymize::createAEx( const AnonInclusionCriteria::Criteria criteria, const AnonType::Type type, const bool useNs)
{
    AnonException *e1 = new AnonException();
    e1->setPath("/a/b/c");
    e1->setFixedValue("123");
    e1->setAnonType(type);
    e1->setCriteria(criteria);
    e1->setUseNamespace(useNs);
    return e1;
}

AnonProfile *TestAnonymize::createProfile()
{
    AnonProfile *profile = new AnonProfile();
    profile->addException(createAEx(AnonInclusionCriteria::StartValue, AnonType::StartValue, false));
    profile->addException(createAEx(AnonInclusionCriteria::StartValue, AnonType::StartValue, true));
    for( int criteria = AnonInclusionCriteria::StartValue;
        criteria < AnonInclusionCriteria::EndValue ; criteria++) {
        for( int type = AnonType::StartValue;
             type < AnonType::EndValue ; type++) {
            profile->addException(createAEx(static_cast<AnonInclusionCriteria::Criteria>(criteria), static_cast<AnonType::Type>(type), false));
        }
    }
    return profile;
}

bool TestAnonymize::testBaseProfile()
{
    _testName = "testBaseProfile" ;
    //serialize
    AnonProfile *profile = createProfile();

    QString xmlProfile = profile->toXMLSerializedString();
    QByteArray byteArray = xmlProfile.toUtf8();
    QBuffer outputData(&byteArray);
    CompareXML compareXML;
    if(!compareXML.compareBufferWithFile(&outputData, PROFILE_XML) ) {
        delete profile;
        return error(QString("Comparing serialized with object:'%1'").arg(compareXML.errorString()));
    }
    AnonProfile newProfile;
    if(!newProfile.readFromSerializedXmlString(xmlProfile)) {
        delete profile;
        return error("unable to deserialize");
    }
    if(!compareProfiles(&newProfile, profile)) {
        delete profile;
        return false;
    }
    return true;
}

bool TestAnonymize::compareProfiles(AnonProfile *p1, AnonProfile *p2)
{
    bool cond ;
    cond = p1->params()->equals(p2->params());
    if(!cond) {
        return error(QString("Params comparison"));
    }
    cond = p1->useNamespace() == p2->useNamespace();
    if(!cond) {
        return error(QString("UseNamespace: %1, %2").arg(p1->useNamespace()).arg(p2->useNamespace()));
    }
    cond = p1->exceptions().count() == p2->exceptions().count();
    if(!cond) {
       return error(QString("exceptions().count: %1, %2").arg(p1->exceptions().count()).arg(p2->exceptions().count()));
    }
    for( int index = 0 ; index < p1->exceptions().count() ; index ++ ) {
        AnonException *e1 = p1->exceptions().at(index);
        AnonException *e2 = p2->exceptions().at(index);
        cond = compareExceptions(index, e1, e2);
        if(!cond) {
            return false;
        }
    }
    return true ;
}

bool TestAnonymize::compareProfilesWrongParams(AnonProfile *p1, AnonProfile *p2)
{
    bool cond ;
    cond = p1->params()->equals(p2->params());
    if(cond) {
        return error(QString("Params comparison"));
    }
    return true ;
}


bool TestAnonymize::compareExceptions(const int index, AnonException *e1, AnonException *e2)
{
    if(e1->criteria() != e2->criteria()) {
        return error(QString("Exception at %1, criteria: %2, %3").arg(index).arg(e1->criteria()).arg(e2->criteria()));
    }
    if(e1->anonType() != e2->anonType()) {
        return error(QString("Exception at %1, anonType: %2, %3").arg(index).arg(e1->anonType()).arg(e2->anonType()));
    }
    if(e1->path() != e2->path()) {
        return error(QString("Exception at %1, path: %2, %3").arg(index).arg(e1->path()).arg(e2->path()));
    }
    if(e1->isUseNamespace() != e2->isUseNamespace()) {
        return error(QString("Exception at %1, useNamespace: %2, %3").arg(index).arg(e1->isUseNamespace()).arg(e2->isUseNamespace()));
    }
    if(e1->fixedValue() != e2->fixedValue()) {
        return error(QString("Exception at %1, fixedValue: %2, %3").arg(index).arg(e1->fixedValue()).arg(e2->fixedValue()));
    }
    return true;
}

bool TestAnonymize::compareProfilesParams(AnonProfile *profile)
{
    QString xmlProfile = profile->toXMLSerializedString();
    AnonProfile newProfile;
    if(!newProfile.readFromSerializedXmlString(xmlProfile)) {
        return error("unable to deserialize");
    }
    if(!compareProfiles(&newProfile, profile)) {
        return false;
    }
    return true ;
}

bool TestAnonymize::compareProfilesParamsInverse(AnonProfile *profile)
{
    QString xmlProfile = profile->toXMLSerializedString();
    AnonProfile newProfile;
    if(!newProfile.readFromSerializedXmlString(xmlProfile)) {
        return error("unable to deserialize");
    }
    profile->params()->useFixedLetter = !profile->params()->useFixedLetter ;
    profile->params()->mode = (AnonymizeParameters::Emodes)(profile->params()->mode + 1) ;
    if(!compareProfilesWrongParams(&newProfile, profile)) {
        return false;
    }
    return true ;
}

bool TestAnonymize::testProfileParams()
{
    _testName = "testProfileParams" ;
    //serialize
    AnonProfile *profile = createProfile();

    AnonymizeParameters *params = profile->params();
    params->mode = AnonymizeParameters::AllText ;
    params->useFixedLetter = false ;
    if(!compareProfilesParams(profile)) {
        delete profile ;
        return error("Compare params 0");
    }
    params->mode = AnonymizeParameters::AllText ;
    params->useFixedLetter = true ;
    if(!compareProfilesParams(profile)) {
        delete profile ;
        return error("Compare params 1");
    }
    params->mode = AnonymizeParameters::UsingPatterns;
    params->useFixedLetter = false ;
    if(!compareProfilesParams(profile)) {
        delete profile ;
        return error("Compare params 2");
    }
    params->mode = AnonymizeParameters::UsingPatterns;
    params->useFixedLetter = true ;
    if(!compareProfilesParams(profile)) {
        delete profile ;
        return error("Compare params 3");
    }
    params->mode = AnonymizeParameters::UsingPatterns;
    params->useFixedLetter = true ;
    if(!compareProfilesParamsInverse(profile)) {
        delete profile ;
        QString msg = this->errorString();
        return error(QString("Compare params false :%1").arg(msg));
    }
    return true ;
}

void TestAnonymize::fillProfile(GenericPersistentData *o1, const QString &newPayload)
{
    o1->setCreationDate(_baseDate);
    o1->setDescription("description");

    o1->setCreationUser("setCreationUser");
    o1->setName("name");
    o1->setPayload(newPayload);
    o1->setReadOnly(false);
    QSet<QString> tags;
    tags << "one";
    tags << "two";
    o1->setTags(tags);
    o1->setUpdateDate(_baseDate);
    o1->setUuid("uuid");
}

bool TestAnonymize::testSaveAndReadProfile()
{
    _testName = "testSaveAndReadProfile" ;
    App app;
    SQLLiteTestAccess access;
    if(!access.init()) {
        return error("init access");
    }
    app.sessionDBPath = access.dbFileName;
    if(!app.init1()) {
        return error("init app");
    }
    AnonProfile *profile = createProfile();
    QString xmlProfile = profile->toXMLSerializedString();

    GenericPersistentData *snippet = app.data()->storageManager()->newPersistentDatum(QXMLE_PDATA_TypeAnonProfile);
    if(NULL == snippet) {
        return error("Unable to create new profile.");
    }
    fillProfile(snippet, xmlProfile);
    OperationStatus* oper = NULL ;
    oper = app.data()->storageManager()->insertGenericData(snippet);
    if((NULL == oper) || !oper->isOk()) {
        return error("Error saving data.");
    }
    delete oper;
    QList<GenericPersistentData*> list;
    oper = app.data()->storageManager()->readGenericData(QXMLE_PDATA_TypeAnonProfile, snippet->id(), list);
    if((NULL == oper) || !oper->isOk()) {
        return error("Error retrieving  data.");
    }
    delete oper;
    if( list.count() != 1 ) {
        error(QString("Read size: %1, expected:1").arg(list.count()));
    }
    if(!isError()) {
        GenericPersistentData *read = list.first();
        if( read->payload() != snippet->payload()) {
            error("Payloads differ.");
        }
        if(!isError()) {
            AnonProfile *readp = new AnonProfile();
            if(!readp->readFromSerializedXmlString(read->payload())) {
                error("Unable to read the profile from string.");
            }
            if(!isError()) {
                if(!compareProfiles(readp, profile)) {
                    _isError = true ;
                }
            }
            delete readp;
        }
    }
    foreach( GenericPersistentData* d, list ) {
        delete d;
    }
    delete profile;
    return !isError();
}

//-------


bool TestAnonymize::testBatch()
{
    _testName = "testBatch";

    if(!testBatchBase()) {
        return false;
    }
    return true;
}


//-------

bool TestAnonymize::testBatchBase()
{
    _testName = "testBatchBase/copy";
    {
        AnonContext context(NULL, "");
        AnonNullAlg *anonNullAlg = new AnonNullAlg();
        context.setAlg(anonNullAlg);
        if(!testBatchBaseSkeleton(ANON_BATCH_COPY1, ANON_BATCH_COPY1, &context) ) {
            return false;
        }
    }
    return true;
}


//----

bool TestAnonymize::testUnitPathBase(const QString &testToken, const QString &fileStart, const QString &spec, QList<int> selection, const QString &expected, const Resolution resolution)
{
    _testName = QString("testUnitPathBase/").append(testToken);
    //----- first part: editor
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(fileStart) ) {
        return error(QString("unable to load input file: '%1' ").arg(fileStart));
    }
    Regola *regola = app.mainWindow()->getRegola();
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(QString("NULL selection app failed:%1").arg(listIntToString(selection)));
    }
    app.mainWindow()->getEditor()->setCurrentItem(selectedElement);
    ElmPathResolver pathResolver;
    pathResolver.select(selectedElement);

    QString pathCalculated ;
    switch( resolution ) {
    case ResolveElement:
        pathCalculated = pathResolver.path();
        break;
    case ResolveAttribute:
        pathCalculated = pathResolver.attributePath(spec);
        break;
    case ResolveText:
        pathCalculated = pathResolver.textPath();
        break;
    default:
        return error(QString("Unknown resolution :%1").arg(resolution));
    }

    if( pathCalculated != expected ) {
        return error(QString("Bad path Found '%1', expected '%2'").arg(pathCalculated).arg(expected));
    }
    return true ;
}


#define FILE_TEST_PATH1            "../test/data/anon/path1.xml"

bool TestAnonymize::testUnitPath()
{
    _testName = "testUnitPath";
    {
        QList<int> selection;
        selection << 1;
        if(!testUnitPathBase("1", FILE_TEST_PATH1, "", selection, "/root", ResolveElement) ) {
            return false;
        }
    }
    {
        QList<int> selection;
        selection << 1 << 0 ;
        if(!testUnitPathBase("2", FILE_TEST_PATH1, "", selection, "/root/{ns1}book", ResolveElement) ) {
            return false;
        }
    }
    {
        QList<int> selection;
        selection << 1 << 0 << 0 ;
        if(!testUnitPathBase("3", FILE_TEST_PATH1, "", selection, "/root/{ns1}book/{ns2}b", ResolveElement) ) {
            return false;
        }
    }
    return true;
}


#define FILE_CSV_IMPORT_EXCEPTIONS                "../test/data/anon/importex.csv"


bool TestAnonymize::testImportExceptions()
{
    _testName = "testImportExceptions";
    App app;
    if(!app.init()) {
                return error("App init");
    }
    Regola regola;
    AnonAdvDialog   dlg(app.data(), &regola);

    if(!dlg.importUsingFile(FILE_CSV_IMPORT_EXCEPTIONS)) {
        return error(QString("Importing CSV data :'%1'").arg(FILE_CSV_IMPORT_EXCEPTIONS));
    }
    AnonProfile *profile = dlg._profile;
    if(NULL == profile ) {
        return error(QString("Null profile"));
    }
    QList<AnonException*> exs = profile->exceptions();
    const int expectedCount = 6 ;
    if( exs.size() != expectedCount ) {
        return error(QString("Expected %1 exceptions, but found :%2").arg(expectedCount).arg(exs.size()));
    }

    // compare one by one
    {
        AnonException *comp = exs.at(0);
        AnonException ex1;
        ex1.setPath("a/b/c");
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 0"));
        }
    }
    {
        AnonException *comp = exs.at(1);
        AnonException ex1;
        ex1.setCriteria(AnonInclusionCriteria::ExcludeWithChildren);
        ex1.setPath("/e/f/@g");
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 1"));
        }
    }

    {
        AnonException *comp = exs.at(2);
        AnonException ex1;
        ex1.setAnonType(AnonType::FixedValue);
        ex1.setCriteria(AnonInclusionCriteria::Include);
        ex1.setPath("c");
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 2"));
        }
    }

    {
        AnonException *comp = exs.at(3);
        AnonException ex1;
        ex1.setAnonType(AnonType::FixedValue);
        ex1.setCriteria(AnonInclusionCriteria::IncludeWithChildren);
        ex1.setFixedValue("aaa");
        ex1.setPath("d");
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 3"));
        }
    }

    {
        AnonException *comp = exs.at(4);
        AnonException ex1;
        ex1.setAnonType(AnonType::FixedValue);
        ex1.setCriteria(AnonInclusionCriteria::IncludeWithChildren);
        ex1.setFixedValue("bbb");
        ex1.setPath("e");
        ex1.setUseNamespace(true);
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 4"));
        }
    }

    {
        AnonException *comp = exs.at(5);
        AnonException ex1;
        ex1.setAnonType(AnonType::FixedValue);
        ex1.setCriteria(AnonInclusionCriteria::Exclude);
        ex1.setFixedValue("ccc");
        ex1.setPath("f");
        ex1.setUseNamespace(false);
        if( !ex1.compare(comp) ) {
            return error(QString("Comparing exception no 5"));
        }
    }
    {
        AnonException *comp = exs.at(5);
        AnonException ex1;
        ex1.setAnonType(AnonType::FixedValue);
        ex1.setCriteria(AnonInclusionCriteria::Exclude);
        ex1.setFixedValue("sss");
        ex1.setPath("c");
        ex1.setUseNamespace(true);
        if( ex1.compare(comp) ) {
            return error(QString("Comparing wrong exception no 5"));
        }
    }
    return true ;
}

#define FILE_EXCEPTIONS "../test/data/anon/exceptions.xml"

bool TestAnonymize::checkgetExc(QList<int> &selection, App *app, const bool expectedNamespace, const QString &expectedPath, const bool isElement )
{
    Regola *regola = app->mainWindow()->getRegola();
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(QString("NULL selection app failed:%1").arg(listIntToString(selection)));
    }
    app->mainWindow()->getEditor()->setCurrentItem(selectedElement);

    bool isError = false ;
    AnonAddExcElementDialog dlg(selectedElement, isElement);
    AnonSettingsWidget *settings = dlg.settingsWidget();

    settings->setUseNamespace(expectedNamespace);
    AnonException *exception = dlg.getException();

    if( exception->isUseNamespace() != expectedNamespace ) {
        isError = error(QString("Use namespace, expected:%1, found:%2").arg(expectedNamespace).arg(exception->isUseNamespace()));
    }
    if( exception->path() != expectedPath ) {
        isError = error(QString("Path expected:%1, found:%2").arg(expectedPath).arg(exception->path()));
    }
    delete exception;
    return !isError;
}

bool TestAnonymize::checkgetAttrExc(QList<int> &selection, App *app, const bool expectedNamespace, const QString &expectedPath)
{
    Regola *regola = app->mainWindow()->getRegola();
    Element *selectedElement = regola->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(QString("NULL selection app failed:%1").arg(listIntToString(selection)));
    }
    app->mainWindow()->getEditor()->setCurrentItem(selectedElement);

    bool isOk = true ;
    AnonAttrExcDialog dlg(selectedElement);
    AnonSettingsWidget *settings = dlg.settingsWidget();

    settings->setUseNamespace(expectedNamespace);
    dlg.selectAllAttributes();
    QList<AnonException*> exList = dlg.getExceptions();
    if( exList.isEmpty() ) {
        return error("Exception list empty");
    }
    AnonException *exception = exList.first();

    if( exception->isUseNamespace() != expectedNamespace ) {
        isOk = error(QString("Use namespace, expected:%1, found:%2").arg(expectedNamespace).arg(exception->isUseNamespace()));
    }
    if( exception->path() != expectedPath ) {
        isOk = error(QString("Path expected:%1, found:%2").arg(expectedPath).arg(exception->path()));
    }
    foreach( AnonException *ex, exList ) {
        delete ex ;
    }
    return isOk;
}

bool TestAnonymize::testInsertException()
{
    _testName = "testInsertException/elements";
    //----- first part: editor
    App app;
    if(!app.init() ) {
        return error("init app failed");
    }
    if( !app.mainWindow()->loadFile(FILE_EXCEPTIONS) ) {
        return error(QString("unable to load input file: '%1' ").arg(FILE_EXCEPTIONS));
    }
    {
        QList<int> selection;
        selection << 0 <<0 << 0 << 0 ;
        if(!checkgetExc(selection, &app, false, "/root/b:bb/c:cc/d", true )) {
            return false;
        }
        if(!checkgetExc(selection, &app, true, "/root/{nsx1}bb/{nsx2}cc/d", true )) {
            return false;
        }

        if(!checkgetExc(selection, &app, false, "/root/b:bb/c:cc/d/text()", false )) {
            return false;
        }
        if(!checkgetExc(selection, &app, true, "/root/{nsx1}bb/{nsx2}cc/d/text()", false)) {
            return false;
        }
    }
    _testName = "testInsertException/attributes";
    {
        QList<int> selection;
        selection << 0 <<0 << 0 << 1 ;
        if(!checkgetAttrExc(selection, &app, false, "/root/b:bb/c:cc/e/@a" )) {
            return false;
        }
        if(!checkgetAttrExc(selection, &app, true, "/root/{nsx1}bb/{nsx2}cc/{nxsx3}e/@a" )) {
            return false;
        }
    }

    return true;
}


bool TestAnonymize::testExportExceptions()
{
    _testName = "testExportExceptions";
    App app;
    if(!app.init()) {
        return error("App init");
    }
    Regola regola;
    AnonAdvDialog   dlg(app.data(), &regola);

    if(!dlg.importUsingFile(FILE_CSV_IMPORT_EXCEPTIONS)) {
        return error(QString("Importing CSV data :'%1'").arg(FILE_CSV_IMPORT_EXCEPTIONS));
    }
    AnonProfile *profile = dlg._profile;
    if(NULL == profile ) {
        return error(QString("Null profile"));
    }
    profile->reset();
    AnonException *exc1 = new AnonException();
    exc1->setPath("/a/b/c");
    exc1->setCriteria(AnonInclusionCriteria::Exclude);
    exc1->setAnonType(AnonType::UseDefault);
    exc1->setUseNamespace(false);
    exc1->setFixedValue("");
    profile->addException(exc1);
    AnonException *exc2 = new AnonException();
    exc2->setPath("x");
    exc2->setCriteria(AnonInclusionCriteria::IncludeWithChildren);
    exc2->setAnonType(AnonType::FixedValue);
    exc2->setUseNamespace(true);
    exc2->setFixedValue("b");
    profile->addException(exc2);
    QApplication::clipboard()->clear();
    dlg.on_cmdExportExceptions_clicked();
    QString clipText = QApplication::clipboard()->text();
    QString expected = "\"/a/b/c\",0,0,\"\",0\n\"x\",3,1,\"b\",1\n";
    if( clipText != expected ) {
        return error(QString("Clipboard is:'%1', expected:'%2'").arg(clipText).arg(expected));
    }
    return true ;

}
