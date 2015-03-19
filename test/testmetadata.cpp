/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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

#include "testmetadata.h"
#include "modules/metadata/metadataparser.h"
#include "modules/metadata/metadatainfo.h"
#include "modules/metadata/metadatainfodialog.h"
#include "app.h"
#include "comparexml.h"

TestMetadata::TestMetadata()
{
}

TestMetadata::~TestMetadata()
{
}

bool TestMetadata::testSuiteParsing()
{
    {
        TestMetadata test;
        bool result = test.testParseEmpty();
        if(!result) {
            return false;
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName", "name");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName1", " name");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName2", " name ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName3", " name '");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName4", " name= ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName5", " name name ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName6", " name=\" value ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName7", " name=\" value ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName8", " name=\" value\"' ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName8", " name=\" value\" name2");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName9", " name=' value");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName10", " name=' value' a");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        bool result = test.testParseExpectingError("onlyName11", " name=' value'' ");
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        QList<QPair<QString,QString> > expected;
        expected.append(QPair<QString,QString>("attr", "test"));
        bool result = test.testParseExpectingSuccess("oneq", " attr=\"test\" ", expected);
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        QList<QPair<QString,QString> > expected;
        expected.append(QPair<QString,QString>("attr", "test"));
        bool result = test.testParseExpectingSuccess("oneq1", "attr  = \"test\" ", expected);
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        QList<QPair<QString,QString> > expected;
        expected.append(QPair<QString,QString>("attr", "test"));
        bool result = test.testParseExpectingSuccess("oneq2", "attr  = 'test' ", expected);
        if(!result) {
            return error(test.errorString());
        }
    }
    {
        TestMetadata test;
        QList<QPair<QString,QString> > expected;
        expected.append(QPair<QString,QString>("attr", "test"));
        expected.append(QPair<QString,QString>("tratt", "tsts"));
        bool result = test.testParseExpectingSuccess("oneq3", "attr  = 'test' tratt=\"tsts\" ", expected);
        if(!result) {
            return error(test.errorString());
        }
    }
    return true;
}

//--------
QString TestMetadata::dumpPair(QList<QPair<QString,QString> > expectedAttributes)
{
    QString result = "\n Dumping pair:\n" ;
    int index = 0;
    QListIterator<QPair<QString,QString> > i(expectedAttributes);
    while (i.hasNext()) {
        QPair<QString,QString>  pair = i.next();
        result += QString(" at:%1 '%2' -> '%3'\n").arg(index).arg(pair.first).arg(pair.second);
        index++ ;
    }
    result += "---\n" ;
    return result;
}

//--------

bool TestMetadata::testParseEmpty()
{
    _testName = "testParseEmpty";
    MetadataParsedResult result;
    MetadataParser parser;
    QString toParse = "";
    if( !parser.parseMetadata(toParse, &result, 0)) {
        return error(QString("unexpected error parsing string '%1'").arg(toParse));
    }
    if( result.attributes().size()>0) {
        return error(QString("found '%1' attributes, but expecting 0.").arg(result.attributes().size()));
    }
    return true;
}


bool TestMetadata::testParseExpectingError(const QString &argTestName, const QString &toParse)
{
    _testName = argTestName ;
    MetadataParsedResult result;
    MetadataParser parser;
    if( parser.parseMetadata(toParse, &result, 0)) {
        return error(QString("expecting error parsing string '%1', but did not").arg(toParse));
    }
    return true;
}

bool TestMetadata::testParseExpectingSuccess(const QString &argTestName, const QString &toParse, QList<QPair<QString,QString> > expectedAttributes)
{
    _testName = argTestName;
    MetadataParsedResult result;
    MetadataParser parser;
    if( !parser.parseMetadata(toParse, &result, 0)) {
        return error(QString("unexpected error parsing string '%1'").arg(toParse));
    }
    if( result.attributes().size() != expectedAttributes.size()) {
        return error(QString("found '%1' attributes, but expecting '%2'. Attributes:'%3', expecting: '%4'").arg(result.attributes().size()).arg(expectedAttributes.size())
                     .arg(result.dumpAttributes()).arg(dumpPair(expectedAttributes)));
    }
    int size = result.attributes().size();
    for( int i = 0 ; i < size ; i ++ ) {
        PseudoAttribute *a = result.attributes().at(i);
        QPair<QString,QString> pair = expectedAttributes.at(i);
        if( a->name() != pair.first ) {
            return error(QString("Result #'%1' differs on name, expecting '%2', found '%3' . Attributes:'%4', expecting: '%5'")
                         .arg(i).arg(a->name()).arg(pair.first)
                         .arg(result.dumpAttributes()).arg(dumpPair(expectedAttributes)));
        }
        if( a->value() != pair.second ) {
            return error(QString("Result #'%1' differs on value, expecting '%2', found '%3' . Attributes:'%4', expecting: '%5'")
                         .arg(i).arg(a->value()).arg(pair.second)
                         .arg(result.dumpAttributes()).arg(dumpPair(expectedAttributes)));
        }
    }

    return true;
}

//--------------------------------------------------------------------------------

/* sequence of tests
class a:
  start | new-open | verify A | cmd ins meta | verify B | undo | verify A | redo | verify B | save | verify mem C, verify file C|

  open:
  new,
  open file,
  new from clipboard,
  new from resource


  save:
  save,
  save as,
  save copy as,
  copy to clipboard,
  save to string

only modifications: !ro+update+save -> c0/1 dep on preamble in ins metadata+save
- !ro+-> ins metadata c0/1 dep on preamble
- !ro+update+ins meta+save -> c0/1 dep on preamble


class b:
 read data | verify | write | verify |
 variables: readonly, update meta, file with meta

 only modification is:
 with data: yes, read only: no, update: enabled

 unificare con dati con meta e fare il riassunto
 */


class ClassATest {
public:
    bool isReadOnly;
    bool isUpdateMeta;
    bool isInsMeta;
    bool isFileMeta;
    bool isInsPreamble;
    int newMethod;
    int saveMethod;
    QString verifyA;
    QString verifyB;
    QString verifyC;

    enum eNew {
        O_OPEN=0,
        //O_NEWFROMCLIPBOARD,
        //O_NEWFROMRESOURCE,
        //O_MAX=O_NEWFROMRESOURCE
        O_MAX=O_OPEN
    };

    enum eSave {
        S_SAVE=0,
        S_SAVEAS,
        S_SAVECOPYAS,
        S_SAVETOSTRING,
        S_MAX=S_SAVETOSTRING
    };
};

//------------------------------------------

class TestMetaDevice : public DocumentDeviceProvider {
public:
    QString currentPath;
    QBuffer buffer;
    TestMetaDevice();
    ~TestMetaDevice();
    QIODevice *newDeviceForWrite(const QString &path);
};

TestMetaDevice::TestMetaDevice(){}
TestMetaDevice::~TestMetaDevice(){}
QIODevice *TestMetaDevice::newDeviceForWrite(const QString & path)
{
    currentPath = path;
    return &buffer;
}
//------------------------------------------
class TestLocalMetaUserMod : public MetadataUserProvider, public MetadataDateProvider
{
    QString user() { return "modusr"; }
    QString date() { return "2014-09-14T18:22:44" ;}
};

class TestLocalMetaUserIns : public MetadataUserProvider, public MetadataDateProvider
{
    QString user() { return "insusr"; }
    QString date() { return "2010-11-09T23:11:45" ;}
};

//----------------------------------------------

#define FILE_PRE_META               "../test/data/metadata/pre_meta.xml"
#define FILE_PRE_META_INS           "../test/data/metadata/pre_meta_ins.xml"
#define FILE_PRE_META_INS_UPD       "../test/data/metadata/pre_meta_ins_upd.xml"
#define FILE_PRE_META_NIN_UPD       "../test/data/metadata/pre_meta_nin_upd.xml"

#define FILE_PRE_NOME               "../test/data/metadata/pre_nome.xml"
#define FILE_PRE_NOME_INS           "../test/data/metadata/pre_nome_ins.xml"
#define FILE_PRE_NOME_INS_UPD       "../test/data/metadata/pre_nome_ins_upd.xml"

#define FILE_NOPRE_META             "../test/data/metadata/nopre_meta.xml"
#define FILE_NOPRE_META_INS         "../test/data/metadata/nopre_meta_ins.xml"
#define FILE_NOPRE_META_INS_UPD     "../test/data/metadata/nopre_meta_ins_upd.xml"
#define FILE_NOPRE_META_NIN_UPD     "../test/data/metadata/nopre_meta_nin_upd.xml"

#define FILE_NOPRE_NOME             "../test/data/metadata/nopre_nome.xml"
#define FILE_NOPRE_NOME_INS         "../test/data/metadata/nopre_nome_ins.xml"
#define FILE_NOPRE_NOME_INS_UPD     "../test/data/metadata/nopre_nome_ins_upd.xml"

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
/*
#define FILE_BASE_1 "../test/data/metadata/base1.xml"
#define FILE_BASEPRE_1 "../test/data/metadata/base1pre.xml"
#define FILE_BASE_META "../test/data/metadata/base1meta.xml"
#define FILE_BASEPRE_META "../test/data/metadata/base1premeta.xml"

#define FILE_BASE_1_RES "../test/data/metadata/base1_res.xml"
#define FILE_BASEPRE_1_RES "../test/data/metadata/base1pre_res.xml"
#define FILE_BASE_META_RES "../test/data/metadata/base1meta_res.xml"
#define FILE_BASEPRE_META_RES "../test/data/metadata/base1premeta_res.xml"

#define FILE_BASE_1_RES "../test/data/metadata/base1_res.xml"
#define FILE_BASEPRE_1_RES "../test/data/metadata/base1pre_res.xml"
#define FILE_BASE_META_RES "../test/data/metadata/base1meta_res.xml"
#define FILE_BASEPRE_META_RES "../test/data/metadata/base1premeta_res.xml"
*/

bool TestMetadata::testManageMeta()
{
    _testName = "testManageMeta";
    QList<bool> bools;
    bools << true << false;
    ClassATest classa;
    foreach( bool bd1, bools ) {
        classa.isReadOnly = bd1;
        foreach( bool bd2, bools ) {
            classa.isUpdateMeta = bd2;
            foreach( bool bd3, bools ) {
                classa.isInsMeta = bd3;
                foreach( bool bd4, bools ) {
                    classa.isInsPreamble = bd4;
                    foreach( bool bd5, bools ) {
                        classa.isFileMeta = bd5;
                        for( int iOpen = 0 ; iOpen <= ClassATest::O_MAX ; iOpen ++ ) {
                            classa.newMethod = iOpen ;
                            for( int iSave = 0 ; iSave <= ClassATest::S_MAX ; iSave ++ ) {
                                classa.saveMethod = iSave ;
                                if( classa.isInsPreamble ) {
                                    if( classa.isFileMeta) {
                                        classa.verifyA = FILE_PRE_META;
                                    } else {
                                        classa.verifyA = FILE_PRE_NOME;
                                    }
                                } else {
                                    if( classa.isFileMeta) {
                                        classa.verifyA = FILE_NOPRE_META;
                                    } else {
                                        classa.verifyA = FILE_NOPRE_NOME;
                                    }
                                }
                                classa.verifyB = classa.verifyA;
                                classa.verifyC = classa.verifyA;

                                if(!classa.isReadOnly&&classa.isInsMeta) {
                                    if( classa.isInsPreamble ) {
                                        if( classa.isFileMeta) {
                                            classa.verifyB = FILE_PRE_META_INS;
                                        } else {
                                            classa.verifyB = FILE_PRE_NOME_INS;
                                        }
                                    } else {
                                        if( classa.isFileMeta) {
                                            classa.verifyB = FILE_NOPRE_META_INS;
                                        } else {
                                            classa.verifyB = FILE_NOPRE_NOME_INS;
                                        }
                                    }
                                }

                                if(!classa.isReadOnly&&classa.isUpdateMeta) {
                                    if( !((classa.saveMethod==ClassATest::S_SAVE)||(classa.saveMethod==ClassATest::S_SAVEAS))) {
                                        classa.verifyC = classa.verifyB ;
                                    } else {
                                        classa.verifyC = classa.verifyB ;
                                        if( classa.isInsPreamble ) {
                                            if( classa.isInsMeta ) {
                                                if( classa.isFileMeta) {
                                                    classa.verifyC = FILE_PRE_META_INS_UPD;
                                                } else {
                                                    classa.verifyC = FILE_PRE_NOME_INS_UPD;
                                                }
                                            } else {
                                                if( classa.isFileMeta) {
                                                    classa.verifyC= FILE_PRE_META_NIN_UPD;
                                                } else {
                                                    classa.verifyC = FILE_PRE_NOME;
                                                }
                                            }
                                        } else {
                                            if( classa.isInsMeta ) {
                                                if( classa.isFileMeta) {
                                                    classa.verifyC = FILE_NOPRE_META_INS_UPD;
                                                } else {
                                                    classa.verifyC = FILE_NOPRE_NOME_INS_UPD;
                                                }
                                            } else {
                                                if( classa.isFileMeta) {
                                                    classa.verifyC = FILE_NOPRE_META_NIN_UPD;
                                                } else {
                                                    classa.verifyC = FILE_NOPRE_NOME;
                                                }
                                            }
                                        }
                                    }
                                } else if (!classa.isReadOnly&&!classa.isUpdateMeta) {
                                    classa.verifyC = classa.verifyB ;
                                }

                                if( !testCycleOnMeta(&classa) ) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true ;
}


QString TestMetadata::describeParams(ClassATest *classa)
{
    QString result = QString("config classA is: isReadOnly:%1, isUpdateMeta:%2, isInsMeta:%3, isInsPreamble:%9, isFileMeta:%10, newMethod:%4, saveMethod:%5, verA:'%6', verB:'%7', verC:'%8'")
        .arg(classa->isReadOnly)//1
        .arg(classa->isUpdateMeta)//2
        .arg(classa->isInsMeta)//3
        .arg(classa->newMethod)//4
        .arg(classa->saveMethod)//5
        .arg(classa->verifyA).arg(classa->verifyB).arg(classa->verifyC)
        .arg(classa->isInsPreamble)//9
        .arg(classa->isFileMeta); //10
    return result;
}
/*
#include <QtGlobal>
static void msgHandler(QtMsgType type, const char *msg)
{
    printf("%s\n", msg);
}
*/

bool TestMetadata::testCycleOnMeta(ClassATest *classa)
{
    //---
    //qInstallMsgHandler(msgHandler);
    //----
    TestLocalMetaUserMod localMetaProviderMod;
    TestLocalMetaUserIns localMetaProviderIns;
    App app;
    if(!app.init()) {
        return error("init app");
    }
    app.data()->setUpdateMetadata(classa->isUpdateMeta);
    switch(classa->newMethod) {
    case ClassATest::O_OPEN:
        if(!app.mainWindow()->loadFile(classa->verifyA) ) {
            return error( QString("error reading file open, state:%1").arg(describeParams(classa)) );
        }
        break;
    default:
        return error( QString("new method not recognized, state:%1").arg(describeParams(classa)) );
    }
    MetadataInfoFactory::setDateProvider(&localMetaProviderIns);
    MetadataInfoFactory::setUserProvider(&localMetaProviderIns);
    app.mainWindow()->getEditor()->setReadOnly(classa->isReadOnly);
    CompareXML compare;
    if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), classa->verifyA )) {
        return error( QString("Verify A, state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }

    //-----

    if( classa->isInsMeta ) {
        if( !fireAction(app.mainWindow(), "actionInsertMetadata" ) ) {
            return error( QString("cannot fire insert metadata, state:%1").arg(describeParams(classa)));
        }
    }
    if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), classa->verifyB)) {
        return error( QString("Verify B (ins metadata), state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }
    app.mainWindow()->getRegola()->undo();
    if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), classa->verifyA)) {
        return error( QString("Verify A (undo), state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }
    app.mainWindow()->getRegola()->redo();
    if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), classa->verifyB)) {
        return error( QString("Verify B (redo), state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }
    MetadataInfoFactory::setDateProvider(&localMetaProviderMod);
    MetadataInfoFactory::setUserProvider(&localMetaProviderMod);

    TestMetaDevice bufInfo;
    app.mainWindow()->getRegola()->setDeviceProvider(&bufInfo);
    QByteArray baLocal;
    switch(classa->saveMethod) {
        case ClassATest::S_SAVE:
            if( !app.mainWindow()->actionSave_internal("") ) {
                return error( QString("saving file S_SAVE state:%1").arg(describeParams(classa)));
            }
            break;
        case ClassATest::S_SAVEAS:
            app.mainWindow()->actionSaveAs_internal("");
            break;
        case ClassATest::S_SAVECOPYAS:
            app.mainWindow()->actionSaveACopyAs_internal("");
            break;
        case ClassATest::S_SAVETOSTRING:
            {
                baLocal = app.mainWindow()->getRegola()->writeMemory();
                bufInfo.buffer.setBuffer(&baLocal);
            }
            break;
        default:
            return error( QString("save method not recognized, state:%1").arg(describeParams(classa)) );
    }
    if(!compare.compareFiles(classa->verifyC, &bufInfo.buffer)) {
        return error( QString("Verify C (memory), state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }

    if(!compare.compareFileWithRegola(app.mainWindow()->getRegola(), classa->verifyC)) {
        return error( QString("Verify C (file), state:%1, cause:%2").arg(describeParams(classa)).arg(compare.errorString()));
    }
    app.mainWindow()->getRegola()->setDeviceProvider(NULL);
    MetadataInfoFactory::setDateProvider(NULL);
    MetadataInfoFactory::setUserProvider(NULL);

    return true;
}

//------------------------------------------------------------

#define OTHER_EMPTY "../test/data/metadata/other_empty.xml"
#define OTHER_FILLED "../test/data/metadata/other_filled.xml"
#define OTHER_NONAME "../test/data/metadata/other_noname.xml"


#define OTHER_ONEVAL    "../test/data/metadata/other_oneval.xml"
#define OTHER_INSERT    "../test/data/metadata/other_insert.xml"
#define OTHER_DELETE    "../test/data/metadata/other_deleted.xml"
#define OTHER_UPDATE    "../test/data/metadata/other_update.xml"

#define OTHER_MIXED_IN  "../test/data/metadata/other_mixedin.xml"
#define OTHER_MIXED_OUT "../test/data/metadata/other_mixedout.xml"

//------------------------------------------------------------

bool TestMetadata::testParseManualEmpty()
{
    _testName = "testParseManualEmpty" ;

    App app;
    if(!app.init() ) {
        return error("init app");
    }

   if( !app.mainWindow()->loadFile(OTHER_EMPTY)) {
       return error(QString("Opening file '%1'").arg(OTHER_EMPTY));
   }
   MetadataInfo info;
   app.mainWindow()->getRegola()->parseMetadata(&info);
   if( !checkMeta(&info, MetadataInfo::ProjectMetaType, false, false, "") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::CopyrightMetaType, false, false, "") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::VersionMetaType, false, false, "") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::DomainMetaType, false, false, "") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::NameMetaType, false, false, "") ) {
       return false;
   }
   return true;
}

bool TestMetadata::testParseManualFilled()
{
    _testName = "testParseManualFilled" ;

    App app;
    if(!app.init() ) {
        return error("init app");
    }

   if( !app.mainWindow()->loadFile(OTHER_FILLED)) {
       return error(QString("Opening file '%1'").arg(OTHER_FILLED));
   }
   MetadataInfo info;
   app.mainWindow()->getRegola()->parseMetadata(&info);
   if( !checkMeta(&info, MetadataInfo::ProjectMetaType, true, true, "ppp&\"") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::CopyrightMetaType, true, true, "ccc&c") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::VersionMetaType, true, true, "vvv") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::DomainMetaType, true, true, "") ) {
       return false;
   }
   if( !checkMeta(&info, MetadataInfo::NameMetaType, true, true, "nnn") ) {
       return false;
   }
   if( checkMeta(&info, MetadataInfo::NameMetaType, true, false, "") ) {
       return error( QString("Attribute '%1', expected failure on read").arg(MetadataInfo::NameMetaType));
   }
   if( checkMeta(&info, MetadataInfo::NameMetaType, false, false, "") ) {
       return error( QString("Attribute '%1', expected failure on value read").arg(MetadataInfo::NameMetaType));
   }
   return true;
}

bool TestMetadata::checkMeta(MetadataInfo *info, const QString &name, const bool read, const bool write, const QString &value)
{
    PseudoAttribute *attr = info->attribute(name);
    if( NULL == attr ) {
        return error( QString("Unable to find the attribute '%1'").arg(name)) ;
    }
    if( attr->exists() != read ) {
        return error( QString("Attribute '%1', read expected %2, found %3").arg(name).arg(read).arg(attr->exists()));
    }
    if( attr->used() != write ) {
        return error( QString("Attribute '%1', write expected %2, found %3").arg(name).arg(write).arg(attr->used()));
    }
    if( attr->value() != value ) {
        return error( QString("Attribute '%1', value expected '%2'', found '%3'").arg(name).arg(value).arg(attr->value()));
    }
    return true ;
}

//----

bool TestMetadata::testEditValuesManual()
{
    _testName = "testEditValuesManual" ;

    App app;
    if(!app.init() ) {
        return error("init app");
    }

   {
       if( !app.mainWindow()->loadFile(OTHER_EMPTY)) {
           return error(QString("Opening file '%1'").arg(OTHER_EMPTY));
       }
       MetadataInfo info;
       app.mainWindow()->getRegola()->parseMetadata(&info);
       MetaDataInfoDialog dialog(false, &info, app.mainWindow());
       dialog.accept();

       if( !checkMeta(&info, MetadataInfo::ProjectMetaType, false, false, "") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::CopyrightMetaType, false, false, "") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::VersionMetaType, false, false, "") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::DomainMetaType, false, false, "") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::NameMetaType, false, false, "") ) {
           return false;
       }
   }
   {
       if( !app.mainWindow()->loadFile(OTHER_FILLED)) {
            return error(QString("Opening file '%1'").arg(OTHER_FILLED));
       }
       MetadataInfo info;
       app.mainWindow()->getRegola()->parseMetadata(&info);
       MetaDataInfoDialog dialog(false, &info, app.mainWindow());
       dialog.accept();

       if( !checkMeta(&info, MetadataInfo::ProjectMetaType, true, true, "ppp&\"") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::CopyrightMetaType, true, true, "ccc&c") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::VersionMetaType, true, true, "vvv") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::DomainMetaType, true, true, "") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::NameMetaType, true, true, "nnn") ) {
           return false;
       }
   }
   {
        if( !app.mainWindow()->loadFile(OTHER_NONAME)) {
            return error(QString("Opening file '%1'").arg(OTHER_NONAME));
        }
       MetadataInfo info;
       app.mainWindow()->getRegola()->parseMetadata(&info);
       MetaDataInfoDialog dialog(false, &info, app.mainWindow());

       dialog.findChild<QCheckBox*>("chkCopy")->click();
       dialog.findChild<QLineEdit*>("editPrject")->setText("newP");
       dialog.findChild<QLineEdit*>("editDomain")->setText("domain");
       dialog.findChild<QCheckBox*>("chkDomain")->click();
       dialog.findChild<QCheckBox*>("chkVersion")->click();
       dialog.findChild<QLineEdit*>("editVersion")->setText("v");
       dialog.accept();

       if( !checkMeta(&info, MetadataInfo::ProjectMetaType, true, true, "newP") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::CopyrightMetaType, true, false, "ccc&c") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::VersionMetaType, false, true, "v") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::DomainMetaType, true, false, "ddd") ) {
           return false;
       }
       if( !checkMeta(&info, MetadataInfo::NameMetaType, false, false, "") ) {
           return false;
       }
   }
   return true;
}

bool TestMetadata::testWriteManualValuesStart(App *app, const QString &argFileName, MetadataInfo *info)
{
    if(!app->init() ) {
        return error("init app");
    }

   if( !app->mainWindow()->loadFile(argFileName)) {
       return error(QString("Opening file '%1'").arg(argFileName));
   }
   app->mainWindow()->getRegola()->parseMetadata(info);
   return true;
}

bool TestMetadata::testWriteManualValuesExec(App *app, const QString &argOrigFileName, const QString &argTestFileName, const QString &label)
{
   {
       CompareXML compare;
       if(!compare.compareFileWithRegola(app->mainWindow()->getRegola(), argTestFileName )) {
           return error( QString("Verify %2 cause:%1").arg(compare.errorString()).arg(label));
       }
   }
   app->mainWindow()->getRegola()->undo();
   {
       CompareXML compare;
       if(!compare.compareAFileWithRegola(argOrigFileName, app->mainWindow()->getRegola())) {
           return error( QString("Verify undo %2, cause:%1").arg(compare.errorString()).arg(label));
       }
   }
   app->mainWindow()->getRegola()->redo();
   {
       CompareXML compare;
       if(!compare.compareAFileWithRegola(argTestFileName,app->mainWindow()->getRegola())) {
           return error( QString("Verify redo %2 cause:%1").arg(compare.errorString()).arg(label));
       }
   }
   return true;
}

//---

bool TestMetadata::testWriteManualValues()
{
    _testName = "testWriteManualValues" ;

    // test insert
    {
        MetadataInfo info;
        App app;
        if(!testWriteManualValuesStart(&app, OTHER_EMPTY, &info) ) {
            return false;
        }
        MetaDataInfoDialog dialog(false, &info, app.mainWindow());

        dialog.findChild<QCheckBox*>("chkProject")->click();
        dialog.findChild<QLineEdit*>("editPrject")->setText("projectinserted");
        dialog.accept();
        app.mainWindow()->getRegola()->applyMetadata(app.mainWindow()->getMainTreeWidget(), &info);
        if(!testWriteManualValuesExec(&app, OTHER_EMPTY, OTHER_INSERT, "ins") ) {
            return false;
        }
    }

    // test delete
    {
        MetadataInfo info;
        App app;
        if(!testWriteManualValuesStart(&app, OTHER_ONEVAL, &info) ) {
            return false;
        }
        MetaDataInfoDialog dialog(false, &info, app.mainWindow());

        dialog.findChild<QCheckBox*>("chkProject")->click();
        dialog.accept();
        app.mainWindow()->getRegola()->applyMetadata(app.mainWindow()->getMainTreeWidget(), &info);
        if(!testWriteManualValuesExec(&app, OTHER_ONEVAL, OTHER_DELETE, "delete") ) {
            return false;
        }
    }
    // test update
    {
        MetadataInfo info;
        App app;
        if(!testWriteManualValuesStart(&app, OTHER_ONEVAL, &info) ) {
            return false;
        }
        MetaDataInfoDialog dialog(false, &info, app.mainWindow());

        dialog.findChild<QLineEdit*>("editPrject")->setText("project updated");
        dialog.accept();
        app.mainWindow()->getRegola()->applyMetadata(app.mainWindow()->getMainTreeWidget(), &info);
        if(!testWriteManualValuesExec(&app, OTHER_ONEVAL, OTHER_UPDATE, "update") ) {
            return false;
        }
    }
    // mixed
    {
        MetadataInfo info;
        App app;
        if(!testWriteManualValuesStart(&app, OTHER_MIXED_IN, &info) ) {
            return false;
        }
        MetaDataInfoDialog dialog(false, &info, app.mainWindow());

        // delete copy
        dialog.findChild<QCheckBox*>("chkCopy")->click();
        // ins project
        dialog.findChild<QCheckBox*>("chkProject")->click();
        dialog.findChild<QLineEdit*>("editPrject")->setText("newP");
        // update domain
        dialog.findChild<QLineEdit*>("editDomain")->setText("updated");

        dialog.accept();
        app.mainWindow()->getRegola()->applyMetadata(app.mainWindow()->getMainTreeWidget(), &info);
        if(!testWriteManualValuesExec(&app, OTHER_MIXED_IN, OTHER_MIXED_OUT, "mixed") ) {
            return false;
        }
    }
    return true;
}

//----

bool TestMetadata::testModifyMeta()
{
    _testName = "testModifyMeta";
    if(!testParseManualEmpty()) {
        return false;
    }
    if(!testParseManualFilled()) {
        return false;
    }
    //--------
    // test read, enable/disable/write /accept/reject
    if(!testEditValuesManual()) {
        return false;
    }
    //--------
    if(!testWriteManualValues()) {
        return false;
    }
    //----
    return true;
}

